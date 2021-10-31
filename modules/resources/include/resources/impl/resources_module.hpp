#ifndef RESOURCES__IMPL__RESOURCES_HPP_
#define RESOURCES__IMPL__RESOURCES_HPP_

#include <resources/resources.hpp>

#include <nodec/signals.hpp>


namespace resources {
namespace impl {

class ResourcesModule : public Resources {
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

public:
    using ResourcePathChangedSignal = nodec::signals::Signal<void(ResourcesModule&, const std::string&)>;

public:
    ResourceRegistry& registry() override {
        return registry_;
    }

    void set_resource_path(const std::string& path) {
        resource_path_changed_(*this, path);
    }

    std::string resource_path() const {
        return internal_resource_path;
    }

public:
    std::string internal_resource_path{ "resources" };

public:
    decltype(auto) resource_path_changed() {
        return resource_path_changed_.signal_interface();
    }

private:
    ResourceRegistry registry_;
    ResourcePathChangedSignal resource_path_changed_;

};

}



}


#endif