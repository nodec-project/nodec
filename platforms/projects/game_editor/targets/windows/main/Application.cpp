#include "Application.hpp"
#include "Window.hpp"
#include "Logging.hpp"

#include <input/keyboard/impl/keyboard_module.hpp>
#include <input/mouse/impl/mouse_module.hpp>
#include <imgui.h>
#include <imwindow/impl/imwindow_impl.hpp>
#include <game_engine/impl/game_engine_module.hpp>
#include <game_editor/impl/menu_impl.hpp>

#include <nodec/object.hpp>


int Application::main_impl()
{

    auto gameEngineModule = nodec::Object::make_holder<game_engine::impl::GameEngineModule>();
    game_engine::impl::current = gameEngineModule;



    Window window(1280, 720,
                  1280, 720,
                  L"Nodec Game Editor",
                  &(gameEngineModule->keyboard_module()),
                  &(gameEngineModule->mouse_module())
    );


    int exitCode;
    while (true)
    {
        if (!Window::ProcessMessages(exitCode))
        {
            break;
        }

        window.Gfx().BeginFrame();

        bool showDemoWindow = true;
        ImGui::ShowDemoWindow(&showDemoWindow);

        game_editor::menu::impl::show_main_menu();
        imwindow::impl::update_windows();

        window.Gfx().EndFrame();

    }


    nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> Program Sucessfully Ending. See you." << std::flush;
    return exitCode;
}