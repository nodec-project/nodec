
#include <scene_set/systems/hierarchy_system.hpp>
#include <scene_set/systems/impl/impl_hierarchy_system.hpp>

namespace scene_set {
namespace systems {

using namespace nodec;


void append_child(SceneRegistry& registry, const SceneEntity parent, const SceneEntity child) {
    registry.emplace_component<components::Hierarchy>(parent);
    registry.emplace_component<components::Hierarchy>(child);

    auto& parent_hier = registry.get_component<components::Hierarchy>(parent);
    auto& child_hier = registry.get_component<components::Hierarchy>(child);

    if (child_hier.parent != entities::null_entity) {
        remove_child(registry, child_hier.parent, child);
    }

    auto grand = parent;
    while (grand != entities::null_entity) {
        if (grand == child) {
            throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
                Formatter() << "Circular reference detected. The given child (entity: " << child
                << ") was detected in the parents of given parent (entity: " << parent << ").",
                __FILE__, __LINE__
                ));
        }
        auto& hier = registry.get_component<components::Hierarchy>(grand);
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

    auto* trfm = registry.try_get_component<components::Transform>(child);
    if (trfm) {
        trfm->dirty = true;
    }
}

void remove_child(SceneRegistry& registry, const SceneEntity parent, const SceneEntity child) {
    auto& parent_hier = registry.get_component<components::Hierarchy>(parent);
    auto& child_hier = registry.get_component<components::Hierarchy>(child);

    auto pos = std::lower_bound(parent_hier.children.begin(),
                                parent_hier.children.end(),
                                child);
    if (pos == parent_hier.children.end() || *pos != child) {
        throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
            Formatter() << "The child (entity: " << child << ") is not a child of the given parent (entity: " << parent << ").",
            __FILE__, __LINE__
            ));
    }

    parent_hier.children.erase(pos);
    child_hier.parent = entities::null_entity;

    auto* trfm = registry.try_get_component<components::Transform>(child);
    if (trfm) {
        trfm->dirty = true;
    }
}

namespace impl {


} // namespace impl


} // namespace systems
} // namespace scene_set