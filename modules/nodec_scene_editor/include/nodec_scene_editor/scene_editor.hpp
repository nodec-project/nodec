#ifndef NODEC_SCENE_EDITOR__SCENE_EDITOR_HPP_
#define NODEC_SCENE_EDITOR__SCENE_EDITOR_HPP_

#include "selection.hpp"

#include <imessentials/window.hpp>
#include <imwindows/entity_inspector_window.hpp>

namespace nodec_scene_editor {

class SceneEditor {
    using EntityInspectorWindow = imwindows::EntityInspectorWindow;

public:
    SceneEditor() = default;

public:
    virtual imessentials::WindowManager &window_manager() = 0;
    virtual Selection &selection() = 0;
    virtual EntityInspectorWindow::ComponentRegistry::RegistryInterface inspector_component_registry() = 0;
};

} // namespace nodec_scene_editor

#endif