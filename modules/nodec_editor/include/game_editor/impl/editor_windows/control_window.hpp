#ifndef GAME_EDITOR__IMPL__EDITOR_WINDOWS__CONTROL_WINDOW_HPP_
#define GAME_EDITOR__IMPL__EDITOR_WINDOWS__CONTROL_WINDOW_HPP_

#include <game_editor/game_editor.hpp>

#include <imelements/window.hpp>
#include <imgui.h>


namespace game_editor {
namespace impl {
namespace editor_windows {


class ControlWindow : public imelements::BaseWindow {

public:
    static void init(imelements::WindowManager& manager, GameEditor* editor) {
        auto& window = manager.get_window<ControlWindow>();
        window.editor_ = editor;
        ImGui::SetWindowFocus(window.name());
    }

public:
    ControlWindow()
        : BaseWindow("Control##EditorWindows", nodec::Vector2f(500, 50)) {

    }

    void on_gui() override {
        if (!editor_) {
            return;
        }

        if (ImGui::Button("Reset")) {
            editor_->reset();
        }

    }

private:
    GameEditor* editor_{ nullptr };
};

}
}
}

#endif