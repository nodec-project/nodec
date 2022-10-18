#pragma once

#include <Graphics/Graphics.hpp>
#include <Rendering/MaterialBackend.hpp>
#include <Rendering/MeshBackend.hpp>
#include <Rendering/ShaderBackend.hpp>
#include <Rendering/TextureBackend.hpp>
#include <Rendering/ImageTexture.hpp>

#include <SceneAudio/AudioClipBackend.hpp>

#include <Font/FontBackend.hpp>
#include <Font/FontLibrary.hpp>

#include <nodec_scene_serialization/archive_context.hpp>
#include <nodec_scene_serialization/serializable_scene_graph.hpp>
#include <nodec_serialization/nodec_rendering/resources/material.hpp>
#include <nodec_serialization/nodec_rendering/resources/mesh.hpp>
#include <nodec_serialization/nodec_rendering/resources/shader.hpp>

#include <nodec/concurrent/thread_pool_executor.hpp>
#include <nodec/resource_management/resource_registry.hpp>

#include <cereal/archives/adapters.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/cereal.hpp>

#include <fstream>

class ResourceLoader {
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

    template<typename T>
    using ResourceFuture = std::future<std::shared_ptr<T>>;

    template<typename T>
    using ResourcePtr = std::shared_ptr<T>;

    using SerializableSceneGraph = nodec_scene_serialization::SerializableSceneGraph;

    static void HandleException(const std::string &identifier) {
        using namespace nodec;

        std::string details;

        try {
            throw;
        } catch (std::exception &e) {
            details = e.what();
        } catch (...) {
            details = "Unknown";
        }

        logging::WarnStream(__FILE__, __LINE__)
            << "Failed to make resource (" << identifier << ")\n"
            << "details: \n"
            << details;
    }

public:
    ResourceLoader(Graphics *pGraphics, ResourceRegistry *pRegistry, FontLibrary *pFontLibrary)
        : mpGraphics{pGraphics}, mpRegistry{pRegistry}, mpFontLibrary{pFontLibrary} {
    }

    // For resource registry
    template<typename Resource, typename ResourceBackend>
    ResourcePtr<Resource> LoadDirect(const std::string &path) {
        std::shared_ptr<Resource> resource = LoadBackend<ResourceBackend>(path);
        return resource;
    }

    template<typename Resource, typename ResourceBackend>
    ResourceFuture<Resource> LoadAsync(const std::string &name, const std::string &path, ResourceRegistry::LoadNotifyer<Resource> notifyer) {
        using namespace nodec;

        return mExecutor.submit(
            [=]() {
                // <https://github.com/microsoft/DirectXTex/issues/163>
                HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
                if (FAILED(hr)) {
                    logging::WarnStream(__FILE__, __LINE__) << "CoInitializeEx failed.";
                }

                std::shared_ptr<Resource> resource = LoadBackend<ResourceBackend>(path);
                notifyer.on_loaded(name, resource);

                CoUninitialize();
                return resource;
            });
    }

    template<typename ResourceBackend>
    std::shared_ptr<ResourceBackend> LoadBackend(const std::string &path) const noexcept;

    template<>
    std::shared_ptr<MeshBackend> LoadBackend<MeshBackend>(const std::string &path) const noexcept {
        using namespace nodec_rendering::resources;
        using namespace nodec;

        std::ifstream file(path, std::ios::binary);

        if (!file) {
            logging::WarnStream(__FILE__, __LINE__) << "Failed to open resource file. path: " << path;
            // return empty mesh.
            return {};
        }

        cereal::PortableBinaryInputArchive archive(file);

        SerializableMesh source;
        try {
            archive(source);
        } catch (...) {
            HandleException(Formatter() << "Mesh::" << path);
            return {};
        }

        auto mesh = std::make_shared<MeshBackend>();
        mesh->vertices.reserve(source.vertices.size());
        for (auto &&src : source.vertices) {
            mesh->vertices.push_back({src.position, src.normal, src.uv, src.tangent});
        }

        mesh->triangles = source.triangles;

        try {
            mesh->update_device_memory(mpGraphics);
        } catch (...) {
            HandleException(Formatter() << "Mesh::" << path);
            return {};
        }

        // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        return mesh;
    }

    template<>
    std::shared_ptr<ShaderBackend> LoadBackend<ShaderBackend>(const std::string &path) const noexcept {
        using namespace nodec;
        using namespace nodec_rendering::resources;

        std::ifstream file(Formatter() << path << "/shader.meta", std::ios::binary);

        if (!file) {
            logging::WarnStream(__FILE__, __LINE__) << "Failed to open resource file. path: " << path;
            // return empty mesh.
            return {};
        }

        cereal::JSONInputArchive archive(file);

        ShaderMetaInfo metaInfo;
        try {
            archive(metaInfo);
        } catch (...) {
            HandleException(Formatter() << "Shader::" << path);
            return {};
        }

        std::vector<SubShaderMetaInfo> subShaderMetaInfos;
        for (const auto &subShaderName : metaInfo.pass) {
            std::ifstream file(Formatter() << path << "/" << subShaderName << ".meta");
            if (!file) return {};
            cereal::JSONInputArchive archive(file);
            SubShaderMetaInfo info;
            try {
                archive(info);
                subShaderMetaInfos.push_back(std::move(info));
            } catch (...) {
                HandleException(Formatter() << "Shader::" << path);
                return {};
            }
        }

        std::shared_ptr<ShaderBackend> shader;
        try {
            shader = std::make_shared<ShaderBackend>(mpGraphics, path, metaInfo, subShaderMetaInfos);
        } catch (...) {
            HandleException(Formatter() << "Shader::" << path);
            return {};
        }

        return shader;
    }

    template<>
    std::shared_ptr<MaterialBackend> LoadBackend<MaterialBackend>(const std::string &path) const noexcept {
        using namespace nodec;
        using namespace nodec::resource_management;
        using namespace nodec_rendering::resources;

        std::ifstream file(path, std::ios::binary);

        if (!file) {
            logging::WarnStream(__FILE__, __LINE__) << "Failed to open resource file. path: " << path;
            // return empty mesh.
            return {};
        }

        SerializableMaterial source;

        try {
            cereal::JSONInputArchive archive(file);
            archive(source);
        } catch (...) {
            HandleException(Formatter() << "Material::" << path);
            return {};
        }

        auto material = std::make_shared<MaterialBackend>(mpGraphics);

        try {
            auto shader = mpRegistry->get_resource_direct<Shader>(source.shader);
            material->set_shader(shader);

            for (auto &&property : source.float_properties) {
                material->set_float_property(property.first, property.second);
            }

            for (auto &&property : source.vector4_properties) {
                material->set_vector4_property(property.first, property.second);
            }

            for (auto &&property : source.texture_properties) {
                auto &sourceEntry = property.second;
                Material::TextureEntry entry;
                entry.sampler = sourceEntry.sampler;

                auto texture = mpRegistry->get_resource_direct<Texture>(sourceEntry.texture);
                entry.texture = texture;

                material->set_texture_entry(property.first, entry);
            }

            material->set_cull_mode(source.cull_mode);
        } catch (...) {
            HandleException(Formatter() << "Material::" << path);
            return {};
        }

        return material;
    }

    template<>
    std::shared_ptr<TextureBackend> LoadBackend<TextureBackend>(const std::string &path) const noexcept {
        using namespace nodec;

        try {
            auto texture = std::make_shared<ImageTexture>(mpGraphics, path);
            return texture;
        } catch (...) {
            HandleException(Formatter() << "Texture::" << path);
            return {};
        }

        return {};
    }

    template<>
    std::shared_ptr<SerializableSceneGraph> LoadBackend<SerializableSceneGraph>(const std::string &path) const noexcept {
        using namespace nodec;
        using namespace nodec_scene_serialization;

        std::ifstream file(path, std::ios::binary);

        if (!file) {
            logging::WarnStream(__FILE__, __LINE__) << "Failed to open resource file. path: " << path;
            // return empty mesh.
            return {};
        }

        ArchiveContext context{*mpRegistry};

        cereal::UserDataAdapter<ArchiveContext, cereal::JSONInputArchive> archive(context, file);

        SerializableSceneGraph graph;

        try {
            archive(graph);
        } catch (...) {
            HandleException(Formatter() << "SerializableSceneGraph::" << path);
            return {};
        }

        auto out = std::make_shared<SerializableSceneGraph>();
        *out = std::move(graph);
        return out;
    }

    template<>
    std::shared_ptr<AudioClipBackend> LoadBackend<AudioClipBackend>(const std::string &path) const noexcept {
        using namespace nodec;

        std::shared_ptr<AudioClipBackend> clip;

        try {
            clip = std::make_shared<AudioClipBackend>(path);
        } catch (...) {
            HandleException(Formatter() << "AudioClip::" << path);
            return {};
        }

        return clip;
    }

    template<>
    std::shared_ptr<FontBackend> LoadBackend(const std::string &path) const noexcept {
        using namespace nodec;

        std::shared_ptr<FontBackend> font;
        try {
            font = std::make_shared<FontBackend>(*mpFontLibrary, path);
        } catch (...) {
            HandleException(Formatter() << "Font::" << path);
            return {};
        }

        return font;
    }

private:
    nodec::concurrent::ThreadPoolExecutor mExecutor;
    Graphics *mpGraphics;
    ResourceRegistry *mpRegistry;
    FontLibrary *mpFontLibrary;
};