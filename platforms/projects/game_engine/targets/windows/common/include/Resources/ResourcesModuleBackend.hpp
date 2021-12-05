#pragma once

#include "ResourceLoader.hpp"

#include <resources/impl/resources_module.hpp>
#include <rendering/resources/mesh.hpp>


class ResourcesModuleBackend : public resources::impl::ResourcesModule {

public:

    void setup_on_boot() {
        resource_path_changed_connection_ = resource_path_changed().connect(
            [](ResourcesModule& resources, const std::string& path) {
                resources.internal_resource_path = path;
            }
        );
    }

    void setup_on_runtime(Graphics* graphics) {
        using namespace nodec;
        using namespace rendering::resources;

        resource_path_changed_connection_.disconnect();

        resource_loader_.reset(new ResourceLoader(graphics, &registry()));


        registry().register_resource_loader<Mesh>(
            [=](auto& name) {
                return resource_loader_->LoadDirect<Mesh, MeshBackend>(Formatter() << resource_path() << "/" << name);
            },
            [=](auto& name, auto notifyer) {
                return resource_loader_->LoadAsync<Mesh, MeshBackend>(name, Formatter() << resource_path() << "/" << name, notifyer);
            });

        registry().register_resource_loader<Shader>(
            [=](auto& name) {
                return resource_loader_->LoadDirect<Shader, ShaderBackend>(Formatter() << resource_path() << "/" << name);
            },
            [=](auto& name, auto notifyer) {
                return resource_loader_->LoadAsync<Shader, ShaderBackend>(name, Formatter() << resource_path() << "/" << name, notifyer);
            });

        registry().register_resource_loader<Material>(
            [=](auto& name) {
                return resource_loader_->LoadDirect<Material, MaterialBackend>(Formatter() << resource_path() << "/" << name);
            },
            [=](auto& name, auto notifyer) {
                return resource_loader_->LoadAsync<Material, MaterialBackend>(name, Formatter() << resource_path() << "/" << name, notifyer);
            });
    }

private:
    ResourcesModule::ResourcePathChangedSignal::Connection
        resource_path_changed_connection_;
    std::unique_ptr<ResourceLoader> resource_loader_;
};