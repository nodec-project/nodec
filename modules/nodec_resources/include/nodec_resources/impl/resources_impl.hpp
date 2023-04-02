#ifndef NODEC_RESOURCES__IMPL__RESOURCES_IMPL_HPP_
#define NODEC_RESOURCES__IMPL__RESOURCES_IMPL_HPP_

#include "../resources.hpp"

#include <nodec/signals/signal.hpp>

namespace nodec_resources {
namespace impl {

class ResourcesImpl : public Resources {
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

public:
    using ResourcePathChangedSignal = nodec::signals::Signal<void(ResourcesImpl &, const std::string &)>;

public:
    ResourceRegistry &registry() override {
        return registry_;
    }

    void set_resource_path(const std::string &path) {
        resource_path_changed_(*this, path);
    }

    std::string resource_path() const {
        return internal_resource_path;
    }

public:
    std::string internal_resource_path{"resources"};

public:
    decltype(auto) resource_path_changed() {
        return resource_path_changed_.signal_interface();
    }

private:
    ResourceRegistry registry_;
    ResourcePathChangedSignal resource_path_changed_;
};

} // namespace impl

} // namespace nodec_resources

#endif