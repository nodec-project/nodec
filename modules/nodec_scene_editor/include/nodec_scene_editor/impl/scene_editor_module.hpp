#ifndef NODEC_SCENE_EDITOR__IMPL__SCENE_EDITOR_MODULE_HPP_
#define NODEC_SCENE_EDITOR__IMPL__SCENE_EDITOR_MODULE_HPP_

#include <imessentials/impl/menu_impl.hpp>
#include <imessentials/impl/window_impl.hpp>
#include <nodec_scene_editor/scene_editor.hpp>

namespace nodec_scene_editor {
namespace impl {

class SceneEditorModule : public SceneEditor {
public:
    SceneEditorModule() {
    }

public:
    imessentials::WindowManager &window_manager() {
        return window_manager_impl_;
    }

    imessentials::impl::WindowManagerImpl &window_manager_impl() {
        return window_manager_impl_;
    }

    Selection &selection() override {
        return selection_;
    }

    EntityInspectorWindow::ComponentRegistry::RegistryInterface inspector_component_registry() override {
        return inspector_component_registry_.registry_interface();
    }

    EntityInspectorWindow::ComponentRegistry &inspector_component_registry_impl() {
        return inspector_component_registry_;
    }

private:
    imessentials::impl::WindowManagerImpl window_manager_impl_;
    Selection selection_;
    EntityInspectorWindow::ComponentRegistry inspector_component_registry_;
};

} // namespace impl
} // namespace nodec_scene_editor

#endif