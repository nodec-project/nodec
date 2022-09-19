#ifndef NODEC_SCENE_SERIALIZATION__ARCHIVE_CONTEXT_HPP_
#define NODEC_SCENE_SERIALIZATION__ARCHIVE_CONTEXT_HPP_

#include <nodec/resource_management/resource_registry.hpp>

namespace nodec_scene_serialization {

class ArchiveContext {
public:
    ArchiveContext(nodec::resource_management::ResourceRegistry &resource_registry)
        : resource_registry_(&resource_registry) {}

    nodec::resource_management::ResourceRegistry &resource_registry() {
        return *resource_registry_;
    }

private:
    nodec::resource_management::ResourceRegistry *resource_registry_;
};
} // namespace nodec_scene_serialization

#endif
