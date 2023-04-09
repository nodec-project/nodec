#ifndef NODEC_SCENE__SYSTEMS__HIERARCHY_SYSTEM_HPP_
#define NODEC_SCENE__SYSTEMS__HIERARCHY_SYSTEM_HPP_

#include "../components/hierarchy.hpp"

#include <nodec/signals/scoped_block.hpp>
#include <nodec/signals/signal.hpp>

#include <cassert>

namespace nodec_scene {
namespace systems {

class HierarchySystem final {
public:
    HierarchySystem(SceneRegistry *registry)
        : registry_{registry} {
        using namespace nodec_scene::components;

        hierarchy_created_connection_ = registry_->component_constructed<Hierarchy>()
                                            .connect(
                                                [&](SceneRegistry &registry, const SceneEntity entity) {
                                                    on_hierarchy_created(registry, entity);
                                                });

        hierarchy_destroyed_connection_ = registry_->component_destroyed<Hierarchy>()
                                              .connect(
                                                  [&](SceneRegistry &registry, const SceneEntity entity) {
                                                      on_hierarchy_destroyed(registry, entity);
                                                  });
    }

    void append_child(const SceneEntity parent, const SceneEntity child) {
        using namespace nodec::entities;
        using namespace nodec;
        using namespace components;

        registry_->emplace_component<Hierarchy>(parent);
        registry_->emplace_component<Hierarchy>(child);

        // Check the circular reference.
        auto grand = parent;
        while (grand != null_entity) {
            if (grand == child) {
                throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                                         << "The entity cannot set itself as a parent. (parent: " << parent
                                         << "; child: " << child << ")");
            }
            auto &hierarchy = registry_->get_component<components::Hierarchy>(grand);
            grand = hierarchy.parent;
        }

        auto &parent_hierarchy = registry_->get_component<Hierarchy>(parent);
        auto &child_hierarchy = registry_->get_component<Hierarchy>(child);

        // If the child is belong to a parent, we need to remove the child from the parent.
        if (child_hierarchy.parent != null_entity) {
            remove_from(registry_->get_component<Hierarchy>(child_hierarchy.parent), child, child_hierarchy);
        } else {
            remove_from(root_hierarchy_, child, child_hierarchy);
        }

        append_last(parent, parent_hierarchy, child, child_hierarchy);

        hierarchy_changed_(parent, child);
    }

    void remove_child(const SceneEntity parent, const SceneEntity child) {
        using namespace nodec::entities;
        using namespace nodec;
        using namespace components;

        auto &parent_hierarchy = registry_->get_component<Hierarchy>(parent);
        auto &child_hierarchy = registry_->get_component<Hierarchy>(child);

        if (child_hierarchy.parent != parent) {
            throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                                     << "The child (entity: " << child << ") is not a child of the given parent (entity: " << parent << ").");
        }

        remove_from(parent_hierarchy, child, child_hierarchy);

        append_last(null_entity, root_hierarchy_, child, child_hierarchy);

        hierarchy_changed_(null_entity, child);
    }

    const components::Hierarchy &root_hierarchy() const noexcept {
        return root_hierarchy_;
    }

    decltype(auto) hierarchy_changed() {
        return hierarchy_changed_.signal_interface();
    }

    void remove_all_children(const SceneEntity parent) {
        using namespace nodec_scene::components;
        using namespace nodec::entities;

        auto &hierarchy = registry_->get_component<Hierarchy>(parent);
        auto child = hierarchy.first;
        while (child != null_entity) {
            auto next = registry_->get_component<Hierarchy>(child).next;
            registry_->destroy_entity(child);
            child = next;
        }
    }

private:
    void append_last(const SceneEntity parent, components::Hierarchy &parent_hierarchy,
                     const SceneEntity child, components::Hierarchy &child_hierarchy) {
        using namespace nodec_scene::components;
        using namespace nodec::entities;

        ++parent_hierarchy.child_count;
        child_hierarchy.parent = parent;
        child_hierarchy.next = null_entity;

        if (parent_hierarchy.first == null_entity) {
            parent_hierarchy.first = child;
        }
        if (parent_hierarchy.last == null_entity) {
            parent_hierarchy.last = child;
            child_hierarchy.prev = null_entity;
        } else {
            auto &last_hierarchy = registry_->get_component<Hierarchy>(parent_hierarchy.last);
            last_hierarchy.next = child;
            child_hierarchy.prev = parent_hierarchy.last;
            parent_hierarchy.last = child;
        }
    }

    void remove_from(components::Hierarchy &parent_hierarchy, const SceneEntity child, components::Hierarchy &child_hierarchy) {
        using namespace nodec_scene::components;
        using namespace nodec::entities;

        if (child_hierarchy.next != null_entity) {
            auto &next_hierarchy = registry_->get_component<Hierarchy>(child_hierarchy.next);
            next_hierarchy.prev = child_hierarchy.prev;
        }
        if (child_hierarchy.prev != null_entity) {
            auto &prev_hierarchy = registry_->get_component<Hierarchy>(child_hierarchy.prev);
            prev_hierarchy.next = child_hierarchy.next;
        }

        --parent_hierarchy.child_count;
        if (parent_hierarchy.first == child) {
            parent_hierarchy.first = child_hierarchy.next;
        }
        if (parent_hierarchy.last == child) {
            parent_hierarchy.last = child_hierarchy.prev;
        }
    }

    void on_hierarchy_created(SceneRegistry &registry, const SceneEntity entity) {
        assert(&registry == registry_);

        using namespace nodec::entities;
        using namespace nodec_scene::components;

        append_last(null_entity, root_hierarchy_, entity, registry_->get_component<Hierarchy>(entity));
    }

    void on_hierarchy_destroyed(SceneRegistry &registry, const SceneEntity entity) {
        assert(&registry == registry_);

        using namespace nodec;
        using namespace nodec::entities;
        using namespace components;

        auto &hierarchy = registry.get_component<Hierarchy>(entity);

        if (hierarchy.parent != null_entity) {
            remove_from(registry_->get_component<Hierarchy>(hierarchy.parent), entity, hierarchy);
        } else {
            remove_from(root_hierarchy_, entity, hierarchy);
        }

        std::vector<SceneEntity> to_deletes = {entity};
        auto opened_offset = 0;
        while (opened_offset < to_deletes.size()) {
            auto curr = registry.get_component<Hierarchy>(to_deletes[opened_offset]).first;
            while (curr != null_entity) {
                to_deletes.emplace_back(curr);
                curr = registry.get_component<Hierarchy>(curr).next;
            }
            ++opened_offset;
        }

        {
            nodec::signals::ScopedBlock<nodec::signals::Connection> block(hierarchy_destroyed_connection_);
            auto iter = to_deletes.begin();
            // The first element is already deleted.
            ++iter;
            for (; iter != to_deletes.end(); ++iter) {
                registry.destroy_entity(*iter);
            }
        }
    }

private:
    SceneRegistry *registry_;

    components::Hierarchy root_hierarchy_;

    nodec::signals::Connection hierarchy_destroyed_connection_;
    nodec::signals::Connection hierarchy_created_connection_;

    nodec::signals::Signal<void(SceneEntity, SceneEntity)> hierarchy_changed_;
};

} // namespace systems
} // namespace nodec_scene

#endif
