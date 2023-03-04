#ifndef NODEC_PHYSICS__COMPONENTS__COLLISION_STAY_HPP_
#define NODEC_PHYSICS__COMPONENTS__COLLISION_STAY_HPP_

#include <nodec_scene/scene_entity.hpp>

namespace nodec_physics {
namespace components {

struct CollisionStay {
    nodec_scene::SceneEntity other;
};

} // namespace components
} // namespace nodec_physics

#endif