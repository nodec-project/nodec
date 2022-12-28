#pragma once

#include "InspectorGUI/InspectorGUI.hpp"

#include <Engine.hpp>

#include <imessentials/impl/menu_impl.hpp>
#include <imessentials/impl/window_impl.hpp>
#include <nodec_scene_editor/impl/scene_editor_module.hpp>

#include <ImGuizmo.h>

class Editor final : public nodec_scene_editor::impl::SceneEditorModule {
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

    void setup();

    void step() {
        if (state_ != State::Paused) return;

        do_one_step_ = true;
    }

    void reset() {
        engine_->world_module().reset();
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

    void update();

    State state() const noexcept {
        return state_;
    }

private:
    Engine *engine_;
    State state_{State::Paused};
    bool do_one_step_{false};

    std::unique_ptr<InspectorGUI> inspector_gui_;
};