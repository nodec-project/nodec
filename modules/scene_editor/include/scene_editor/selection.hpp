#ifndef SCENE_EDITOR__SELECTION_HPP_
#define SCENE_EDITOR__SELECTION_HPP_

#include <scene_set/scene_registry.hpp>

namespace scene_editor {


struct Selection {
    scene_set::SceneEntity activeSceneEntity = nodec::entities::null_entity;
};

}

#endif