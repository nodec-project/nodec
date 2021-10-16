#pragma once

#include "../Editor.hpp"

#include <imelements/window.hpp>
#include <imgui.h>


class ControlWindow : public imelements::BaseWindow {

public:
    static void init(imelements::WindowManager& manager, Editor* editor) {
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


        switch (editor_->mode()) {
        case Editor::Mode::Edit:
            if (ImGui::Button("Play Mode")) {
                editor_->enter_playmode();
            }
            break;

        case Editor::Mode::Play:
            if (ImGui::Button("Edit Mode")) {
                editor_->exit_playmode();
            }
            break;
        }

        switch (editor_->state()) {
        case Editor::State::Paused:
            ImGui::SameLine();
            if (ImGui::Button("Play")) {
                editor_->play();
            }
            ImGui::SameLine();
            if (ImGui::Button("Step")) {
                editor_->step();
            }
            break;

        case Editor::State::Playing:
            ImGui::SameLine();
            if (ImGui::Button("Pause")) {
                editor_->pause();
            }
            break;
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            editor_->reset();
        }

    }

private:
    Editor* editor_{ nullptr };
    bool edit_mode_{ false };
};


