#ifndef SCENE_EDITOR__SCENE_EDITOR_HPP_
#define SCENE_EDITOR__SCENE_EDITOR_HPP_

#include "selection.hpp"

#include <imelements/window.hpp>
#include <imwindows/entity_inspector_window.hpp>


namespace scene_editor {

class SceneEditor {
    using EntityInspectorWindow = imwindows::EntityInspectorWindow;

public:
    SceneEditor() = default;

public:
    virtual imelements::WindowManager& window_manager() = 0;
    virtual Selection& selection() = 0;
    virtual EntityInspectorWindow::ComponentRegistry::RegistryInterface inspector_component_registry() = 0;

};




}

#endif