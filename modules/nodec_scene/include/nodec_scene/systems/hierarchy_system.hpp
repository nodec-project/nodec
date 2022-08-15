#ifndef NODEC_SCENE__SYSTEMS__HIERARCHY_SYSTEM_HPP_
#define NODEC_SCENE__SYSTEMS__HIERARCHY_SYSTEM_HPP_

#include "../components/basic.hpp"

#include <cassert>

namespace nodec_scene {
namespace systems {

class HierarchySystem {
    using RegistryConnection = nodec::signals::Signal<void(SceneRegistry &, const SceneEntity)>::Connection;

public:
    HierarchySystem(SceneRegistry *registry)
        : registry_{registry} {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[HierarchySystem] >>> init()";

        hierarchy_created_connection_ = registry_->component_constructed<components::Hierarchy>()
                                            .connect(
                                                [&](SceneRegistry &registry, const SceneEntity entity) {
                                                    on_hierarchy_created(registry, entity);
                                                })
                                            .assign();

        hierarchy_destroyed_connection_ = registry_->component_destroyed<components::Hierarchy>()
                                              .connect(
                                                  [&](SceneRegistry &registry, const SceneEntity entity) {
                                                      on_hierarchy_destroyed(registry, entity);
                                                  })
                                              .assign();
    }

    const std::vector<SceneEntity> &root_entities() const {
        return root_entities_;
    }

    void append_child(const SceneEntity parent, const SceneEntity child) {
        using namespace nodec::entities;
        using namespace nodec;

        registry_->emplace_component<components::Hierarchy>(parent);
        registry_->emplace_component<components::Hierarchy>(child);

        auto &parent_hier = registry_->get_component<components::Hierarchy>(parent);
        auto &child_hier = registry_->get_component<components::Hierarchy>(child);

        if (child_hier.parent != null_entity) {
            remove_child(child_hier.parent, child);
        }

        auto grand = parent;
        while (grand != null_entity) {
            if (grand == child) {
                throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                                         << "The entity cannot set itself as a parent. (parent: " << parent
                                         << "; child: " << child << ")");
            }
            auto &hier = registry_->get_component<components::Hierarchy>(grand);
            grand = hier.parent;
        }

        auto pos = std::lower_bound(parent_hier.children.begin(),
                                    parent_hier.children.end(),
                                    child);

        if (pos != parent_hier.children.end() && *pos == child) {
            // already exists
            return;
        }

        parent_hier.children.insert(pos, child);
        child_hier.parent = parent;
        remove_root(child);

        auto *trfm = registry_->try_get_component<components::Transform>(child);
        if (trfm) {
            trfm->dirty = true;
        }
    }

    void remove_child(const SceneEntity parent, const SceneEntity child) {
        using namespace nodec::entities;
        using namespace nodec;

        auto &parent_hier = registry_->get_component<components::Hierarchy>(parent);
        auto &child_hier = registry_->get_component<components::Hierarchy>(child);

        auto pos = std::lower_bound(parent_hier.children.begin(),
                                    parent_hier.children.end(),
                                    child);
        if (pos == parent_hier.children.end() || *pos != child) {
            throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                                     << "The child (entity: " << child << ") is not a child of the given parent (entity: " << parent << ").");
        }

        parent_hier.children.erase(pos);
        child_hier.parent = null_entity;
        append_root(child);

        auto *trfm = registry_->try_get_component<components::Transform>(child);
        if (trfm) {
            trfm->dirty = true;
        }
    }

private:
    void append_root(const SceneEntity entity) {
        auto pos = std::lower_bound(root_entities_.begin(), root_entities_.end(), entity);
        if (pos != root_entities_.end() && *pos == entity) {
            // already exists
            return;
        }
        root_entities_.insert(pos, entity);
    }

    void remove_root(const SceneEntity entity) {
        auto pos = std::lower_bound(root_entities_.begin(), root_entities_.end(), entity);
        if (pos != root_entities_.end() && *pos == entity) {
            root_entities_.erase(pos);
            return;
        }
    }

    void on_hierarchy_created(SceneRegistry &registry, const SceneEntity entity) {
        assert(&registry == registry_);

        append_root(entity);
    }

    void on_hierarchy_destroyed(SceneRegistry &registry, const SceneEntity entity) {
        assert(&registry == registry_);

        auto &hier = registry.get_component<components::Hierarchy>(entity);

        if (hier.parent != nodec::entities::null_entity) {
            auto &parent_hier = registry.get_component<components::Hierarchy>(hier.parent);
            remove_child(hier.parent, entity);
        }

        std::vector<SceneEntity> to_deletes = {entity};
        auto opened_offset = 0;
        while (opened_offset < to_deletes.size()) {
            auto &hier = registry.get_component<components::Hierarchy>(to_deletes[opened_offset]);
            std::copy(hier.children.begin(), hier.children.end(), std::back_inserter(to_deletes));
            ++opened_offset;
        }

        {
            nodec::signals::ScopedBlock<RegistryConnection> block(hierarchy_destroyed_connection_);
            auto iter = to_deletes.begin();
            remove_root(*iter);
            ++iter;
            for (; iter != to_deletes.end(); ++iter) {
                registry.destroy_entity(*iter);
                remove_root(*iter);
            }
        }
    }

private:
    SceneRegistry *registry_;

    std::vector<SceneEntity> root_entities_;

    RegistryConnection hierarchy_destroyed_connection_;
    RegistryConnection hierarchy_created_connection_;
};

} // namespace systems
} // namespace nodec_scene

#endif
