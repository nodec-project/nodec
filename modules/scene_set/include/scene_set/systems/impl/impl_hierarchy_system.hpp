#ifndef SCENE_SET__SYSTEMS__IMPL__IMPL_HIERARCHY_SYSTEM_HPP_
#define SCENE_SET__SYSTEMS__IMPL__IMPL_HIERARCHY_SYSTEM_HPP_

#include<scene_set/systems/hierarchy_system.hpp>
#include<scene_set/scene_registry.hpp>
#include<nodec/logging.hpp>

#include <mutex>

namespace scene_set {
namespace systems {
namespace impl {


class HierarchySystem {
    using RegistryConnection = nodec::signals::Signal<void(SceneRegistry&, const SceneEntity)>::Connection;

public:
    void init(SceneRegistry& registry) {
        on_destroy_connection
            = registry.on_destroy<components::Hierarchy>().connect(
                [&](SceneRegistry& registry, const SceneEntity entity) {
                    handle_hierarchy_remove(registry, entity);
                })
            .assign();
    }

private:
    void handle_hierarchy_remove(SceneRegistry& registry, const SceneEntity entity) {
        auto& hier = registry.get_component<components::Hierarchy>(entity);

        if (hier.parent != nodec::entities::null_entity) {
            auto& parent_hier = registry.get_component<components::Hierarchy>(hier.parent);
            remove_child(registry, hier.parent, entity);
        }

        std::vector<SceneEntity> to_deletes = { entity };
        auto opened_offset = 0;
        while (opened_offset < to_deletes.size()) {
            auto& hier = registry.get_component<components::Hierarchy>(to_deletes[opened_offset]);
            std::copy(hier.children.begin(), hier.children.end(), std::back_inserter(to_deletes));
            ++opened_offset;
        }

        {
            nodec::signals::ScopedBlock<RegistryConnection> block(on_destroy_connection);
            for (auto entt : to_deletes) {
                registry.destroy_entity(entt);
            }
        }
    }

private:
    RegistryConnection on_destroy_connection;
};


}
}
}

#endif