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

        engine->setup();

        editor.reset(new Editor(engine.get()));
        engine->add_module<SceneEditor>(editor);
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