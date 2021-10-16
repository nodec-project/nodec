#pragma once

#include "Editor.hpp"

#include <WinDesktopApplication.hpp>
#include <Engine.hpp>
#include <Window.hpp>

#include <scene_editor/impl/scene_editor_module.hpp>



class Application : public WinDesktopApplication {

public:
    Application() {};

protected:

    void setup() {
        using namespace nodec;
        using namespace scene_editor::impl;
        using namespace scene_editor;

        engine.reset(new Engine);
        engine->configure();

        engine->setup();

        editor.reset(new Editor(engine.get()));
        engine->register_module<SceneEditor>(editor.get());
    }

    void loop() {
        engine->frame_begin();

        editor->update();

        engine->frame_end();
    }

private:
    std::unique_ptr<Engine> engine;
    std::unique_ptr<Editor> editor;


};