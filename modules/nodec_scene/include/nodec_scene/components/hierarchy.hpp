#ifndef NODEC_SCENE__COMPONENTS__HIERARCHY_HPP_
#define NODEC_SCENE__COMPONENTS__HIERARCHY_HPP_

#include "../scene_registry.hpp"

#include <vector>

namespace nodec_scene {
namespace components {

struct Hierarchy {
    std::vector<SceneEntity> children;
    SceneEntity parent{nodec::entities::null_entity};
};

}
}

#endif