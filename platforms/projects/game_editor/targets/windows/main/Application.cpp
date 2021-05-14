#include "Application.hpp"
#include "EditorScreenHandlers.hpp"

#include <Window.hpp>
#include <Logging.hpp>

#include <input/keyboard/impl/keyboard_module.hpp>
#include <input/mouse/impl/mouse_module.hpp>
#include <imgui.h>
#include <imwindow/impl/imwindow_impl.hpp>
#include <game_engine/impl/game_engine_module.hpp>
#include <game_editor/impl/menu_impl.hpp>

#include <nodec/error_formatter.hpp>


int Application::main_impl() {
    using namespace nodec;

    logging::InfoStream(__FILE__, __LINE__)
        << "[Main] >>> launch the Engine.";

    game_engine::impl::GameEngineModule gameEngineModule;
    game_engine::impl::set_current(&gameEngineModule);

    EditorScreenHandlers screenHandlers;
    screenHandlers.SetupOnBootingHandlers(gameEngineModule.screen_module());

    logging::InfoStream(__FILE__, __LINE__)
        << "[Main] >>> boot the Engine.";

    if (!game_engine::impl::boot(gameEngineModule)) {
        throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
            "Failed to boot the engine.",
            __FILE__, __LINE__
            ));
    }


    Window window(gameEngineModule.screen_module().size_internal.x, 
                  gameEngineModule.screen_module().size_internal.y,
                  gameEngineModule.screen_module().resolution_internal.x,
                  gameEngineModule.screen_module().resolution_internal.y,
                  L"Nodec Game Editor",
                  &(gameEngineModule.keyboard_module()),
                  &(gameEngineModule.mouse_module())
    );
    window.SetTitle(gameEngineModule.screen_module().title_internal + " - Editor Mode");

    screenHandlers.SetupRuntimeHandlers(gameEngineModule.screen_module(), &window);
    
    int exitCode;
    while (true) {
        if (!Window::ProcessMessages(exitCode)) {
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