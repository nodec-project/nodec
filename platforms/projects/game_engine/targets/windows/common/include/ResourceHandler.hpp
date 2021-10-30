#pragma once

#include <nodec/resource_management/resource_registry.hpp>
#include <nodec/concurrent/thread_pool_executor.hpp>

#include "Rendering/Mesh.hpp"


class ResourceHandler {

    using ResourceRegistry = nodec::resource_management::ResourceRegistry;
    
    template<typename T>
    using ResourceFuture = std::future<std::shared_ptr<T>>;

public:
    void BindRegistry(ResourceRegistry &registry) {
        //registry.register_resource_loader<rendering::resources::Mesh>([=](const std::string& name, ResourceRegistry::ResourceLoadBridge))

    }


    ResourceFuture<rendering::resources::Mesh> MeshLoader(const std::string& name, 
                                                          ResourceRegistry::LoadBridge<rendering::resources::Mesh> loadBrige) {


    }
    
private:
    nodec::concurrent::ThreadPoolExecutor mExecutor;

};