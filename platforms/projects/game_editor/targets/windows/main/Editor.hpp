#pragma once

#include "InspectorGUI/InspectorGUI.hpp"

#include <Engine.hpp>

#include <imelements/impl/menu_impl.hpp>
#include <imelements/impl/window_impl.hpp>
#include <scene_editor/impl/scene_editor_module.hpp>

class Editor : public scene_editor::impl::SceneEditorModule {
public:
    enum class Mode {
        Edit,
        Play
    };

    enum class State {
        Playing,
        Paused
    };

public:
    Editor(Engine *engine);

    ~Editor() {
    }

    void step() {
        if (state_ != State::Paused) return;

        do_one_step_ = true;
    }

    void reset() {
        engine_->scene_module().reset();
        state_ = State::Paused;
    }

    void play() {
        if (state_ == State::Playing) return;

        state_ = State::Playing;
    }

    void pause() {
        if (state_ == State::Paused) return;

        state_ = State::Paused;
    }

    void update() {
        switch (state_) {
        case State::Playing:
            engine_->scene_module().step();
            break;

        case State::Paused:

            if (do_one_step_) {
                engine_->scene_module().step();
                do_one_step_ = false;
            }

            break;
        }

        imelements::impl::show_main_menu();

        window_manager_impl().update_windows();

        bool showDemoWindow = true;
        ImGui::ShowDemoWindow(&showDemoWindow);
    }

    void enter_playmode() noexcept {
        mode_ = Mode::Play;
    }

    void exit_playmode() noexcept {
        mode_ = Mode::Edit;
    }

    State state() const noexcept {
        return state_;
    }

    Mode mode() const noexcept {
        return mode_;
    }

private:
    Engine *engine_;
    State state_{State::Paused};
    Mode mode_{Mode::Edit};
    bool do_one_step_{false};

    std::unique_ptr<InspectorGUI> inspector_gui_;
};