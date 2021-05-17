#include "Application.hpp"
#include "EditorScreenHandlers.hpp"

#include <Window.hpp>
#include <Logging.hpp>

#include <input/keyboard/impl/keyboard_module.hpp>
#include <input/mouse/impl/mouse_module.hpp>
#include <imgui.h>
#include <game_engine/impl/game_engine_module.hpp>
#include <game_editor/impl/game_editor_module.hpp>

#include <nodec/error_formatter.hpp>


int Application::main_impl() {
    using namespace nodec;

    logging::InfoStream(__FILE__, __LINE__)
        << "[Main] >>> Launch the Editor.";
    game_editor::impl::GameEditorModule editorModule;


    logging::InfoStream(__FILE__, __LINE__)
        << "[Main] >>> launch the Engine.";
    game_engine::impl::GameEngineModule engineModule;
    game_engine::impl::set_current(&engineModule);

    
    EditorScreenHandlers screenHandlers;
    screenHandlers.SetupOnBootingHandlers(engineModule.screen_module());

    logging::InfoStream(__FILE__, __LINE__)
        << "[Main] >>> boot the Engine.";

    if (!game_engine::impl::boot(engineModule)) {
        throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
            "Failed to boot the Engine.",
            __FILE__, __LINE__
            ));
    }


    Window window(engineModule.screen_module().resolution_internal.x,
                  engineModule.screen_module().resolution_internal.y,
                  engineModule.screen_module().resolution_internal.x,
                  engineModule.screen_module().resolution_internal.y,
                  L"Nodec Game Editor",
                  &(engineModule.keyboard_module()),
                  &(engineModule.mouse_module())
    );
    window.SetTitle(engineModule.screen_module().title_internal + " - Editor Mode");

    screenHandlers.SetupRuntimeHandlers(engineModule.screen_module(), &window);
    
    int exitCode;
    while (true) {
        if (!Window::ProcessMessages(exitCode)) {
            break;
        }

        window.Gfx().BeginFrame();
        

        bool showDemoWindow = true;
        ImGui::ShowDemoWindow(&showDemoWindow);


        imelements::impl::show_main_menu();
        editorModule.window_manager_impl().update_windows();

        //game_editor::menu::impl::show_main_menu();
        //imwindow::impl::update_windows();
        //

        window.Gfx().EndFrame();

    }


    nodec::logging::InfoStream(__FILE__, __LINE__) 
        << "[Main] >>> Program Sucessfully Ending. See you." << std::flush;
    return exitCode;
}