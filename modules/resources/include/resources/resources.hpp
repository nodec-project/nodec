#ifndef RESOURCES__RESOURCES_HPP_
#define RESOURCES__RESOURCES_HPP_

#include <nodec/resource_management/resource_registry.hpp>


namespace resources {

class Resources {
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

public:
    virtual ResourceRegistry& registry() = 0;

    virtual void set_resource_path(const std::string& path) = 0;

    virtual std::string resource_path() const = 0;

};


}


#endif