#ifndef NODEC_RESOURCES__RESOURCES_HPP_
#define NODEC_RESOURCES__RESOURCES_HPP_

#include <nodec/resource_management/resource_registry.hpp>

namespace nodec_resources {

class Resources {
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

public:
    virtual ResourceRegistry &registry() = 0;

    virtual void set_resource_path(const std::string &path) = 0;

    virtual std::string resource_path() const = 0;
};

} // namespace nodec_resources

#endif