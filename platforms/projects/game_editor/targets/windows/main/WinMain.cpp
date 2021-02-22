#include "Window.hpp"
#include "Logging.hpp"


#include <nodec_modules/input/keyboard/keyboard_module.hpp>
#include <nodec_modules/input/mouse/mouse_module.hpp>
#include <imgui.h>
#include <imwindow/impl/imwindow_impl.hpp>

#include <Windows.h>



int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    try
    {
        // --- Init Logging ---

        InitLogging(nodec::logging::Level::Debug);

        // end Init Logging ---

        auto keyboard_module = nodec::NodecObject::instanciate<nodec_modules::input::keyboard::KeyboardModule>();
        auto mouse_module = nodec::NodecObject::instanciate<nodec_modules::input::mouse::MouseModule>();

        Window window(1280, 720,
                      1280, 720,
                      L"Nodec Game Editor",
                      keyboard_module.get(),
                      mouse_module.get()
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

            imwindow::impl::show_window_main_menu();

            window.Gfx().EndFrame();

        }


        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> Program Sucessfully Ending. See you." << std::flush;
        return exitCode;
    }
    catch (const std::exception& e)
    {
        nodec::logging::fatal(e.what(), __FILE__, __LINE__);
    }
    catch (...)
    {
        nodec::logging::fatal("Unknown Error Exception Ocuurs.", __FILE__, __LINE__);
    }
    MessageBox(nullptr,
               L"Unhandled Exception has been caught in main loop. \nFor more detail, Please check the 'output.log'.",
               L"Fatal Error.", MB_OK | MB_ICONEXCLAMATION);
    nodec::logging::WarnStream(__FILE__, __LINE__) << "[Main] >>> Unexpected Program Ending." << std::flush;
    return -1;
}