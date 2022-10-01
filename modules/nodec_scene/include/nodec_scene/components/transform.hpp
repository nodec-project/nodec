#ifndef NODEC_SCENE__COMPONENTS__TRANSFORM_HPP_
#define NODEC_SCENE__COMPONENTS__TRANSFORM_HPP_

#include <nodec/matrix4x4.hpp>
#include <nodec/quaternion.hpp>
#include <nodec/vector3.hpp>

namespace nodec_scene {
namespace components {

struct Transform {
    nodec::Vector3f local_position{nodec::Vector3f::zero};
    nodec::Vector3f local_scale{nodec::Vector3f::ones};
    nodec::Quaternionf local_rotation{nodec::Quaternionf::identity};

    nodec::Matrix4x4f local2world;
    bool dirty{true};
};
} // namespace components
} // namespace nodec_scene

#endif