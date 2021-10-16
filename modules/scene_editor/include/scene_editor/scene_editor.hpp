#ifndef SCENE_EDITOR__SCENE_EDITOR_HPP_
#define SCENE_EDITOR__SCENE_EDITOR_HPP_

#include "selection.hpp"

#include <imelements/window.hpp>


namespace scene_editor {

class SceneEditor {
public:
    SceneEditor() = default;

public:
    virtual imelements::WindowManager& window_manager() = 0;
    virtual const Selection& selection() = 0;

};




}

#endif