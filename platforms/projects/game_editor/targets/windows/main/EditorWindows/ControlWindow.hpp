#pragma once

#include "../Editor.hpp"

#include <imessentials/window.hpp>
#include <imgui.h>


class ControlWindow : public imessentials::BaseWindow {

public:
    //static void init(imessentials::WindowManager& manager, ) {
    //    ImGui::SetWindowFocus(window.name());
    //}

public:
    ControlWindow(Editor *editor)
        : BaseWindow("Control##EditorWindows", nodec::Vector2f(500, 50)),
          editor_(editor) {

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


