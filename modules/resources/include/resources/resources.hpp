#ifndef RESOURCES__RESOURCES_HPP_
#define RESOURCES__RESOURCES_HPP_

#include <nodec/resource_management/resource_registry.hpp>


namespace resources {

class Resources {
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

public:

    ResourceRegistry& registry() {
        return registry_;
    }

private:
    ResourceRegistry registry_;


};


}


#endif