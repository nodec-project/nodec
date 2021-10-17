#ifndef SCENE_EDITOR__IMPL__SCENE_EDITOR_MODULE_HPP_
#define SCENE_EDITOR__IMPL__SCENE_EDITOR_MODULE_HPP_

#include <scene_editor/scene_editor.hpp>
#include <imelements/impl/window_impl.hpp>
#include <imelements/impl/menu_impl.hpp>


namespace scene_editor {
namespace impl {


class SceneEditorModule : public SceneEditor {
public:
    SceneEditorModule() {

    }

public:

    imelements::WindowManager& window_manager() {
        return window_manager_impl_;
    }

    imelements::impl::WindowManagerImpl& window_manager_impl() {
        return window_manager_impl_;
    }

    Selection& selection() override {
        return selection_;
    }


private:
    imelements::impl::WindowManagerImpl window_manager_impl_;
    Selection selection_;
    



};

}
}

#endif