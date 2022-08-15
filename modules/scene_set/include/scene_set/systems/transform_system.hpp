#ifndef SCENE_SET__SYSTEMS__TRANSFORM_SYSTEM_HPP_
#define SCENE_SET__SYSTEMS__TRANSFORM_SYSTEM_HPP_

#include <scene_set/components/basic.hpp>
#include <scene_set/scene_registry.hpp>

#include <nodec/math/gfx.hpp>
#include <nodec/matrix4x4.hpp>

namespace scene_set {
namespace systems {

namespace internal {

inline void update_transform(SceneRegistry &registry,
                             const nodec::Matrix4x4f &model_mat,
                             components::Transform &trfm,
                             components::Hierarchy &hier,
                             bool dirty) {
    using namespace nodec;
    using namespace scene_set::components;

    // logging::InfoStream info(__FILE__, __LINE__);

    dirty |= trfm.dirty;
    if (dirty) {
        trfm.local2world = model_mat * math::gfx::trs(trfm.local_position, trfm.local_rotation, trfm.local_scale);
        // info << trfm.local2world;
        trfm.dirty = false;
    }

    for (auto &child : hier.children) {
        auto comps = registry.try_get_components<Transform, Hierarchy>(child);
        if (!(std::get<0>(comps) && std::get<1>(comps))) {
            // required both transform and hierarchy to compute transform. if not, skip the child.
            continue;
        }

        update_transform(registry, trfm.local2world, *std::get<0>(comps), *std::get<1>(comps), dirty);
    }
}

} // namespace internal

inline void update_transform(SceneRegistry &registry, SceneEntity root) {
    using namespace components;
    using namespace nodec;

    auto comps = registry.try_get_components<Transform, Hierarchy>(root);
    if (!(std::get<0>(comps) && std::get<1>(comps))) {
        return;
    }

    internal::update_transform(registry, Matrix4x4f::identity, *std::get<0>(comps), *std::get<1>(comps), false);
}

} // namespace systems
} // namespace scene_set

#endif