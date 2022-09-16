#pragma once

#include "ResourceLoader.hpp"

#include <nodec_resources/impl/resources_module.hpp>

class ResourcesModuleBackend : public nodec_resources::impl::ResourcesModule {
public:
    void setup_on_boot() {
        resource_path_changed_connection_ = resource_path_changed().connect(
            [](ResourcesModule &resources, const std::string &path) {
                resources.internal_resource_path = path;
            });
    }

    void setup_on_runtime(Graphics *graphics, FontLibrary *font_library) {
        using namespace nodec;
        using namespace nodec_rendering::resources;
        using namespace nodec_scene_serialization;
        using namespace nodec_scene_audio::resources;

        resource_path_changed_connection_.disconnect();

        resource_loader_.reset(new ResourceLoader(graphics, &registry(), font_library));

        registry().register_resource_loader<Mesh>(
            [=](auto &name) {
                return resource_loader_->LoadDirect<Mesh, MeshBackend>(Formatter() << resource_path() << "/" << name);
            },
            [=](auto &name, auto notifyer) {
                return resource_loader_->LoadAsync<Mesh, MeshBackend>(name, Formatter() << resource_path() << "/" << name, notifyer);
            });

        registry().register_resource_loader<Shader>(
            [=](auto &name) {
                return resource_loader_->LoadDirect<Shader, ShaderBackend>(Formatter() << resource_path() << "/" << name);
            },
            [=](auto &name, auto notifyer) {
                return resource_loader_->LoadAsync<Shader, ShaderBackend>(name, Formatter() << resource_path() << "/" << name, notifyer);
            });

        registry().register_resource_loader<Texture>(
            [=](auto &name) {
                return resource_loader_->LoadDirect<Texture, TextureBackend>(Formatter() << resource_path() << "/" << name);
            },
            [=](auto &name, auto notifyer) {
                return resource_loader_->LoadAsync<Texture, TextureBackend>(name, Formatter() << resource_path() << "/" << name, notifyer);
            });

        registry().register_resource_loader<Material>(
            [=](auto &name) {
                return resource_loader_->LoadDirect<Material, MaterialBackend>(Formatter() << resource_path() << "/" << name);
            },
            [=](auto &name, auto notifyer) {
                return resource_loader_->LoadAsync<Material, MaterialBackend>(name, Formatter() << resource_path() << "/" << name, notifyer);
            });

        registry().register_resource_loader<SerializableSceneGraph>(
            [=](auto &name) {
                return resource_loader_->LoadDirect<SerializableSceneGraph, SerializableSceneGraph>(Formatter() << resource_path() << "/" << name);
            },
            [=](auto &name, auto notifyer) {
                return resource_loader_->LoadAsync<SerializableSceneGraph, SerializableSceneGraph>(name, Formatter() << resource_path() << "/" << name, notifyer);
            });

        registry().register_resource_loader<AudioClip>(
            [=](auto &name) {
                return resource_loader_->LoadDirect<AudioClip, AudioClipBackend>(Formatter() << resource_path() << "/" << name);
            },
            [=](auto &name, auto notifyer) {
                return resource_loader_->LoadAsync<AudioClip, AudioClipBackend>(name, Formatter() << resource_path() << "/" << name, notifyer);
            });

        registry().register_resource_loader<Font>(
            [=](auto &name) {
                return resource_loader_->LoadDirect<Font, FontBackend>(Formatter() << resource_path() << "/" << name);
            },
            [=](auto &name, auto notifyer) {
                return resource_loader_->LoadAsync<Font, FontBackend>(name, Formatter() << resource_path() << "/" << name, notifyer);
            });
    }

private:
    ResourcesModule::ResourcePathChangedSignal::Connection
        resource_path_changed_connection_;
    std::unique_ptr<ResourceLoader> resource_loader_;
};