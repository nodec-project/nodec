#ifndef GAME_EDITOR__SELECTION_HPP_
#define GAME_EDITOR__SELECTION_HPP_

#include <scene_set/scene_registry.hpp>

namespace game_editor {


struct Selection {
    scene_set::SceneEntity activeSceneEntity = nodec::entities::null_entity;
};

}

#endif