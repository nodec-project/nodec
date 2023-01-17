#ifndef NODEC_SCENE__SCENE_REGISTRY_HPP_
#define NODEC_SCENE__SCENE_REGISTRY_HPP_

#include "scene_entity.hpp"

#include <nodec/entities/registry.hpp>

namespace nodec_scene {

using SceneRegistry = nodec::entities::BasicRegistry<SceneEntity>;

} // namespace nodec_scene

#endif