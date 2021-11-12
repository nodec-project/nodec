#pragma once

#include "ResourceLoader.hpp"

#include <resources/impl/resources_module.hpp>
#include <rendering/resources/mesh.hpp>


class ResourcesModuleBackend : public resources::impl::ResourcesModule {

    using Mesh = rendering::resources::Mesh;

public:

    ResourcesModuleBackend(ResourceLoader* resource_loader) {
        using namespace nodec;

        registry().register_resource_loader<Mesh>(
            [=](auto& name, auto bridge) {
                return resource_loader->LoadMeshAsync(name, Formatter() << resource_path() << "/" << name, bridge);
            });
    }

    void bind_handlers_on_boot() {
        resource_path_changed_connection_ = resource_path_changed().connect(
            [](ResourcesModule& resources, const std::string& path) {
                resources.internal_resource_path = path;
            }
        );
    }

    void bind_handers_on_runtime() {
        resource_path_changed_connection_.disconnect();
    }

private:
    ResourcesModule::ResourcePathChangedSignal::Connection
        resource_path_changed_connection_;

    //ResourceLoader* resource_loader_;
};