#ifndef NODEC_RENDERING__SYSTEMS__VISIBILITY_SYSTEM_HPP_
#define NODEC_RENDERING__SYSTEMS__VISIBILITY_SYSTEM_HPP_

#include "../components/non_visible.hpp"

#include <nodec_scene/components/hierarchy.hpp>
#include <nodec_scene/scene_registry.hpp>

namespace nodec_rendering {
namespace systems {

class VisibilitySystem {
    using ComponentSignal = nodec::signals::Signal<void(nodec_scene::SceneRegistry &, const nodec_scene::SceneEntity)>;

public:
    VisibilitySystem(nodec_scene::SceneRegistry *scene_registry) {
        using namespace nodec::signals;
        using namespace nodec::entities;
        using namespace components;
        using namespace nodec_scene;
        using namespace nodec_scene::components;

        constructed_conn_ = scene_registry->component_constructed<NonVisible>().connect(
            [=](SceneRegistry &registry, const SceneEntity entity) {
                ScopedBlock<ComponentSignal::Connection> block(constructed_conn_);

                std::vector<SceneEntity> stack;
                stack.emplace_back(entity);

                while (!stack.empty()) {
                    auto entt = stack.back();
                    stack.pop_back();

                    auto *hierarchy = registry.try_get_component<Hierarchy>(entt);
                    if (hierarchy == nullptr) return;

                    auto child = hierarchy->first;

                    while (child != null_entity) {
                        registry.emplace_component<NonVisible>(child);

                        stack.emplace_back(child);
                        child = registry.get_component<Hierarchy>(child).next;
                    }
                }
            });

        destroyed_conn_ = scene_registry->component_destroyed<NonVisible>().connect(
            [=](SceneRegistry &registry, const SceneEntity entity) {
                ScopedBlock<ComponentSignal::Connection> block(destroyed_conn_);

                std::vector<SceneEntity> stack;
                stack.emplace_back(entity);

                while (!stack.empty()) {
                    auto entt = stack.back();
                    stack.pop_back();

                    auto *hierarchy = registry.try_get_component<Hierarchy>(entt);
                    if (hierarchy == nullptr) return;

                    auto child = hierarchy->first;

                    while (child != null_entity) {
                        auto *non_visible = registry.try_get_component<NonVisible>(child);

                        if (non_visible && non_visible->self) {
                            child = registry.get_component<Hierarchy>(child).next;
                            continue;
                        }

                        registry.remove_component<NonVisible>(child);

                        stack.emplace_back(child);
                        child = registry.get_component<Hierarchy>(child).next;
                    }
                }
            });
    }

private:
    ComponentSignal::Connection constructed_conn_;
    ComponentSignal::Connection destroyed_conn_;
};
} // namespace systems
} // namespace nodec_rendering

#endif