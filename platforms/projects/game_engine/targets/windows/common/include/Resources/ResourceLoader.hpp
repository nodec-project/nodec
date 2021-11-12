#pragma once

#include <Graphics/Graphics.hpp>
#include <Rendering/MeshBackend.hpp>

#include <serialization/rendering/resources/mesh.hpp>

#include <nodec/resource_management/resource_registry.hpp>
#include <nodec/concurrent/thread_pool_executor.hpp>

#define CEREAL_THREAD_SAFE 1
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <fstream>

class ResourceLoader {

    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

    template<typename T>
    using ResourceFuture = std::future<std::shared_ptr<T>>;

    using Mesh = rendering::resources::Mesh;

    static void HandleException(const std::string &identifier) {
        using namespace nodec;

        std::string details;

        try {
            throw;
        }
        catch (std::exception& e) {
            details = e.what();
        }
        catch (...) {
            details = "Unknown";
        }

        logging::WarnStream(__FILE__, __LINE__)
            << "Failed to make resource (" << identifier << ")\n"
            << "details: \n"
            << details;
    }

public:

    void Setup(Graphics *pGraphics) {
        mpGraphics = pGraphics;
    }


    // For resource registry
    ResourceFuture<Mesh> LoadMeshAsync(const std::string& name, const std::string& path, ResourceRegistry::LoadNotifyer<Mesh> notifyer) {
        return mExecutor.submit(
            [=]() {
                std::shared_ptr<Mesh> mesh = LoadMesh(path);
                notifyer.on_loaded(name, mesh);
                return mesh;
            });
    }


    //ResourceFuture<MeshBackend> LoadMeshAync(const std::string& path) {

    //}


    std::shared_ptr<MeshBackend> LoadMesh(const std::string path) const noexcept {
        assert(mpGraphics && "Call the setup function before use.");

        using namespace rendering::resources;
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
        }
        catch (...) {
            HandleException(Formatter() << "Mesh::" << path);
        }

        auto mesh = std::make_shared<MeshBackend>();
        mesh->vertices.reserve(source.vertices.size());
        for (auto&& src : source.vertices) {
            mesh->vertices.push_back({ src.position, src.normal, src.uv, src.tangent });
        }

        mesh->triangles = source.triangles;

        try {
            mesh->update_device_memory(mpGraphics);
        }
        catch (...) {
            HandleException(Formatter() << "Mesh::" << path);
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        return mesh;
    }


private:
    nodec::concurrent::ThreadPoolExecutor mExecutor;
    Graphics* mpGraphics;

};