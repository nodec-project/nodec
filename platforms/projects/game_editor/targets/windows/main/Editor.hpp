#pragma once


#include <Engine.hpp>

#include <scene_editor/impl/scene_editor_module.hpp>
#include <imelements/impl/menu_impl.hpp>
#include <imelements/impl/window_impl.hpp>


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
    Editor(Engine* engine);

    ~Editor() {

    }

    void step() {
        if (state_ != State::Paused) {
            return;
        }

        do_one_step_ = true;
    }

    void reset() {
        engine_->reset();
        state_ = State::Paused;
    }

    void play() {
        if (state_ == State::Playing) {
            return;
        }

        state_ = State::Playing;
    }

    void pause() {
        if (state_ == State::Paused) {
            return;
        }

        state_ = State::Paused;
    }

    void update() {

        switch (state_) {
        case State::Playing:
            engine_->step();
            break;

        case State::Paused:

            if (do_one_step_) {
                engine_->step();
                do_one_step_ = false;
            }

            break;
        }


        imelements::impl::show_main_menu();
        window_manager_impl().update_windows();

        bool showDemoWindow = true;
        ImGui::ShowDemoWindow(&showDemoWindow);
    }

    void enter_playmode() {

        mode_ = Mode::Play;

    }

    void exit_playmode() {

        mode_ = Mode::Edit;

    }

    State state() const {
        return state_;
    }

    Mode mode() const {
        return mode_;
    }

private:
    Engine* engine_;
    State state_{ State::Paused };
    Mode mode_{ Mode::Edit };
    bool do_one_step_{ false };

};