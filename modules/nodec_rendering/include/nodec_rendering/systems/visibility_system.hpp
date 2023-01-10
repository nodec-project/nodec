#ifndef NODEC_RENDERING__SYSTEMS__VISIBILITY_SYSTEM_HPP_
#define NODEC_RENDERING__SYSTEMS__VISIBILITY_SYSTEM_HPP_

#include "../components/non_visible.hpp"

#include <nodec_scene/components/hierarchy.hpp>
#include <nodec_scene/scene.hpp>

namespace nodec_rendering {
namespace systems {

inline void set_visible(nodec_scene::SceneRegistry &registry, nodec_scene::SceneEntity entity, bool visible) {
    using namespace nodec_scene::components;
    using namespace nodec_rendering::components;
    using namespace nodec::entities;

    if (visible) {
        bool parent_non_visible = [&]() {
            auto *hierarchy = registry.try_get_component<Hierarchy>(entity);
            if (hierarchy == nullptr) return false;
            if (hierarchy->parent == null_entity) return false;

            return registry.try_get_component<NonVisible>(hierarchy->parent) != nullptr;
        }();

        if (parent_non_visible) {
            auto &non_visible = registry.emplace_component<NonVisible>(entity).first;
            non_visible.self = false;
            return;
        } else {
            registry.remove_component<NonVisible>(entity);
            return;
        }
    } else {
        auto &non_visible = registry.emplace_component<NonVisible>(entity).first;
        non_visible.self = true;
    }
}

class VisibilitySystem {
public:
    VisibilitySystem(nodec_scene::Scene &scene) {
        using namespace nodec::signals;
        using namespace nodec::entities;
        using namespace components;
        using namespace nodec_scene;
        using namespace nodec_scene::components;

        constructed_conn_ = scene.registry().component_constructed<NonVisible>().connect(
            [&](SceneRegistry &registry, const SceneEntity entity) {
                ScopedBlock<nodec::signals::Connection> block(constructed_conn_);

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

        destroyed_conn_ = scene.registry().component_destroyed<NonVisible>().connect(
            [&](SceneRegistry &registry, const SceneEntity entity) {
                ScopedBlock<nodec::signals::Connection> block(destroyed_conn_);

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

        scene.hierarchy_system().hierarchy_changed().connect(
            [&](SceneEntity parent, SceneEntity child) {
                auto &registry = scene.registry();
                auto *parent_non_visible = (parent == null_entity)
                                               ? nullptr
                                               : registry.try_get_component<NonVisible>(parent);
                auto *child_non_visible = registry.try_get_component<NonVisible>(child);

                if (parent_non_visible && !child_non_visible) {
                    registry.emplace_component<NonVisible>(child);
                }
                if (!parent_non_visible && child_non_visible && !child_non_visible->self) {
                    registry.remove_component<NonVisible>(child);
                }
            });
    }

private:
    nodec::signals::Connection constructed_conn_;
    nodec::signals::Connection destroyed_conn_;
};
} // namespace systems
} // namespace nodec_rendering

#endif