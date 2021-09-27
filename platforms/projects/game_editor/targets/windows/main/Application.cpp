#include "Application.hpp"
#include "Engine.hpp"
#include "Window.hpp"

//#include "EditorScreenHandlers.hpp"
//
//#include <Window.hpp>
//#include <Logging.hpp>
//
//#include <input/keyboard/impl/keyboard_module.hpp>
//#include <input/mouse/impl/mouse_module.hpp>
//#include <imgui.h>
//#include <game_engine/impl/game_engine_module.hpp>
//#include <game_editor/impl/game_editor_module.hpp>

#include <nodec/unicode.hpp>


int Application::main_impl() {
    using namespace nodec;

    Engine engine;
    engine.configure();

    mpWindow.reset(new Window(engine.screen_module().internal_resolution.x, engine.screen_module().internal_resolution.y,
                              engine.screen_module().internal_resolution.x, engine.screen_module().internal_resolution.y,
                              unicode::utf8to16<std::wstring>(engine.screen_module().internal_title).c_str()));


    //mpWindow.reset(new Window(1280, 720,
    //                          1280, 720,
    //                          unicode::utf8to16<std::wstring>(engine.screenModule().internal_title).c_str()));

    engine.setup(mpWindow.get());

    int exitCode;
    while (true) {
        if (!Window::ProcessMessages(exitCode)) {
            break;
        }

        //window.Gfx().BeginFrame();
        //

        //window.Gfx().EndFrame();

    }


    return exitCode;

    //logging::InfoStream(__FILE__, __LINE__)
    //    << "[Main] >>> Launch the Editor.";
    //game_editor::impl::GameEditorModule editorModule;


    ////logging::InfoStream(__FILE__, __LINE__)
    ////    << "[Main] >>> launch the Engine.";
    ////game_engine::impl::GameEngineModule engineModule;
    ////game_engine::impl::set_current(&engineModule);

    //
    //EditorScreenHandlers screenHandlers;
    //screenHandlers.SetupOnBootingHandlers(editorModule.engine_module().screen_module());

    //logging::InfoStream(__FILE__, __LINE__)
    //    << "[Main] >>> boot the Engine.";

    //if (!editorModule.engine_module().boot()) {
    //    throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
    //        "Failed to boot the Engine.",
    //        __FILE__, __LINE__
    //        ));
    //}


    //Window window(editorModule.engine_module().screen_module().resolution_internal.x,
    //              editorModule.engine_module().screen_module().resolution_internal.y,
    //              editorModule.engine_module().screen_module().resolution_internal.x,
    //              editorModule.engine_module().screen_module().resolution_internal.y,
    //              L"Nodec Game Editor",
    //              &(editorModule.engine_module().keyboard_module()),
    //              &(editorModule.engine_module().mouse_module())
    //);
    //window.SetTitle(editorModule.engine_module().screen_module().title_internal + " - Editor Mode");

    //screenHandlers.SetupRuntimeHandlers(editorModule.engine_module().screen_module(), &window);
    //
    //int exitCode;
    //while (true) {
    //    if (!Window::ProcessMessages(exitCode)) {
    //        break;
    //    }

    //    window.Gfx().BeginFrame();
    //    

    //    bool showDemoWindow = true;
    //    ImGui::ShowDemoWindow(&showDemoWindow);


    //    imelements::impl::show_main_menu();
    //    editorModule.window_manager_impl().update_windows();

    //    //game_editor::menu::impl::show_main_menu();
    //    //imwindow::impl::update_windows();
    //    //

    //    window.Gfx().EndFrame();

    //}


    //nodec::logging::InfoStream(__FILE__, __LINE__) 
    //    << "[Main] >>> Program Sucessfully Ending. See you." << std::flush;
    //return exitCode;
}