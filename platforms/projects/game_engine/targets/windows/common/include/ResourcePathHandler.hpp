#pragma once

#include <resources/impl/resources_module.hpp>

class ResourcePathHandler {
    using ResourcesModule = resources::impl::ResourcesModule;

public:

    ResourcePathHandler(ResourcesModule* pResourcesModule)
        : mpResourcesModule(pResourcesModule) {

    }

    void BindHandlersOnBoot() {
        mResourcePathChangedConnection = mpResourcesModule->resource_path_changed().connect(
            [](ResourcesModule& resources, const std::string& path) {
                resources.internal_resource_path = path;
            }
        );
    }

    void BindHandlersOnRuntime() {
        mResourcePathChangedConnection.disconnect();
    }

private:
    ResourcesModule* mpResourcesModule;

    ResourcesModule::ResourcePathChangedSignal::Connection
        mResourcePathChangedConnection;
};