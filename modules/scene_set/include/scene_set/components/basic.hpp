#ifndef SCENE_SET__COMPONENTS__STANDARD_HPP_
#define SCENE_SET__COMPONENTS__STANDARD_HPP_

#include <scene_set/scene_registry.hpp>

#include <nodec/vector3.hpp>
#include <nodec/quaternion.hpp>
#include <nodec/matrix4x4.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace scene_set {
namespace components {

struct Name {
    std::string name;
};


struct Hierarchy {
    std::vector<SceneEntity> children;
    SceneEntity parent{ nodec::entities::null_entity };
};


struct Transform {
    nodec::Vector3f local_position{ nodec::Vector3f::zero };
    nodec::Vector3f local_scale{ nodec::Vector3f::ones };
    nodec::Quaternionf local_rotation{ nodec::Quaternionf::identity };

    nodec::Matrix4x4f local2world;
    bool dirty{ true };
};

}
}

#endif