#pragma once

#include "Editor.hpp"

#include <Engine.hpp>
#include <WinDesktopApplication.hpp>
#include <Window.hpp>

#include <nodec_scene_editor/impl/scene_editor_module.hpp>

class Application : public WinDesktopApplication {
public:
    Application(){};

protected:
    void setup() {
        using namespace nodec;
        using namespace nodec_scene_editor::impl;
        using namespace nodec_scene_editor;

        engine.reset(new Engine);

        editor.reset(new Editor(engine.get()));
        engine->add_module<SceneEditor>(editor);

        engine->configure();

        // ImGui does not work if the resolution and window size are not the same.
        engine->screen_module().internal_size = engine->screen_module().internal_resolution;

        engine->setup();
        editor->setup();

        // Do first step (initialize).
        engine->world_module().reset();
    }

    void loop() {
        engine->frame_begin();

        editor->update();

        engine->frame_end();
    }

private:
    std::unique_ptr<Engine> engine;
    std::shared_ptr<Editor> editor;
};