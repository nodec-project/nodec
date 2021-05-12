#ifndef SCENE_SET__SYSTEMS__HIERARCHY_SYSTEM_HPP_
#define SCENE_SET__SYSTEMS__HIERARCHY_SYSTEM_HPP_

#include <scene_set/components/standard.hpp>

namespace scene_set {
namespace systems {

void append_child(SceneRegistry& registry, const SceneEntity parent, const SceneEntity child);
void remove_child(SceneRegistry& registry, const SceneEntity parent, const SceneEntity child);

}
}

#endif
