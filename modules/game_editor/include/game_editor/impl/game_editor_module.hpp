#ifndef GAME_EDITOR__IMPL__GAME_EDITOR_MODULE_HPP_
#define GAME_EDITOR__IMPL__GAME_EDITOR_MODULE_HPP_

#include <game_editor/game_editor.hpp>
#include <imelements/impl/window_impl.hpp>
#include <imelements/impl/menu_impl.hpp>


namespace game_editor {
namespace impl {


class GameEditorModule : public GameEditor {
public:
    GameEditorModule();

public:
    imelements::WindowManager& window_manager() {
        return window_manager_impl_;
    }

    imelements::impl::WindowManagerImpl& window_manager_impl() {
        return window_manager_impl_;
    }

private:
    imelements::impl::WindowManagerImpl window_manager_impl_;
};

}
}

#endif