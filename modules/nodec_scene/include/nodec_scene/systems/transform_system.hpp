#ifndef NODEC_SCENE__SYSTEMS__TRANSFORM_SYSTEM_HPP_
#define NODEC_SCENE__SYSTEMS__TRANSFORM_SYSTEM_HPP_

#include <cassert>

#include <nodec/math/gfx.hpp>
#include <nodec/math/math.hpp>
#include <nodec/matrix4x4.hpp>

#include "../components/hierarchy.hpp"
#include "../components/local_to_world.hpp"
#include "../components/local_transform.hpp"
#include "../scene_registry.hpp"

namespace nodec_scene {
namespace systems {

namespace internal {

inline void update_transform(SceneRegistry &registry,
                             SceneEntity entity,
                             const nodec::Matrix4x4f &parent_local_to_world,
                             components::LocalToWorld &local_to_world,
                             components::Hierarchy &hierarchy,
                             bool dirty) {
    using namespace nodec;
    using namespace nodec::entities;
    using namespace nodec_scene::components;

    auto *local_trfm = registry.try_get_component<LocalTransform>(entity);

    if (local_trfm && local_trfm->dirty) {
        local_to_world.value = parent_local_to_world * math::gfx::trs(local_trfm->position, local_trfm->rotation, local_trfm->scale);
        local_trfm->dirty = false;
        local_to_world.dirty = false;
        dirty = true;
    } else if (local_to_world.dirty) {
        local_to_world.dirty = false;
        dirty = true;

        // update local transform if any.
        if (local_trfm) {
            auto delta = math::inv(parent_local_to_world) * local_to_world.value;
            if (!math::gfx::decompose_trs(delta, local_trfm->position, local_trfm->rotation, local_trfm->scale)) {
                assert(false);
            }
        }
    } else {
        if (dirty && local_trfm) {
            local_to_world.value = parent_local_to_world * math::gfx::trs(local_trfm->position, local_trfm->rotation, local_trfm->scale);
        }
    }

    auto child = hierarchy.first;
    while (child != null_entity) {
        auto comps = registry.try_get_components<LocalToWorld, Hierarchy>(child);
        // required both transform and hierarchy to compute transform. if not, skip the child.
        if (!(std::get<0>(comps) && std::get<1>(comps))) continue;

        update_transform(registry, child, local_to_world.value, *std::get<0>(comps), *std::get<1>(comps), dirty);
        child = std::get<1>(comps)->next;
    }
}

} // namespace internal

inline void update_transform(SceneRegistry &registry, SceneEntity root) {
    using namespace components;
    using namespace nodec;

    auto comps = registry.try_get_components<LocalToWorld, Hierarchy>(root);
    if (!(std::get<0>(comps) && std::get<1>(comps))) return;

    internal::update_transform(registry, root, Matrix4x4f::identity, *std::get<0>(comps), *std::get<1>(comps), false);
}

} // namespace systems
} // namespace nodec_scene

#endif