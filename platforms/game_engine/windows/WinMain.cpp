#include "Graphics/GraphicsResources.hpp"
#include "Graphics/RenderingHandlers.hpp"
#include "Graphics/RenderingUtils.hpp"
#include "Graphics/GraphicsRenderer.hpp"
#include "Logging.hpp"
#include "Utils.hpp"
#include "Window.hpp"

#include <nodec_modules/game_engine/game_engine_module.hpp>

#include <nodec/logging.hpp>
#include <nodec/nodec_exception.hpp>
#include <nodec/unicode.hpp>

#include <Windows.h>
#include <exception>
#include <fstream>


int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{

    try
    {
#ifndef NDEBUG
        InitLogging(nodec::logging::Level::Debug);
#else
        InitLogging(nodec::logging::Level::Info);
#endif
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> Hello world. Program start." << std::flush;


        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> launch the Engine." << std::flush;
        auto game_engine_module = nodec_modules::game_engine::get_game_engine_module();


        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> launch the window and graphics." << std::flush;
        Window window(1280, 720, 
                      L"TEST", 
                      &(game_engine_module->keyboard_module())
        );

        //window.SetTitle("ほげabほげ");
        GraphicsResources graphicsResources;
        GraphicsRenderer graphicsRenderer(&window.Gfx());

        auto renderingHandlers = std::make_shared<RenderingHandlers>(&window.Gfx(), &graphicsResources, &graphicsRenderer);
        RenderingUtils::InitRenderingHandlers(renderingHandlers, game_engine_module->rendering_module());

        //game_engine_module.keyboard().test = 10;
        //MessageBox(nullptr, std::to_wstring(game_engine_module.keyboard().test).c_str(), L"", MB_OK | MB_ICONEXCLAMATION);
        //game_engine_module.keyboard_module().test = 100;
        //MessageBox(nullptr, std::to_wstring(game_engine_module.keyboard().test).c_str(), L"", MB_OK | MB_ICONEXCLAMATION);



        game_engine_module->engine_time_stopwatch().lap();

        int exit_code;
        while (true)
        {
            // process all messages pending, but to not block for new messages
            if (!Window::ProcessMessages(exit_code))
            {
                // if ProcessMessages() returns false, means we're quiting so break
                break;
            }
            window.Gfx().BeginFrame();
            //window.Gfx().DrawTestTriangle();

            game_engine_module->rendering_module().frame_delta_time_ = std::chrono::duration<float>(game_engine_module->engine_time_stopwatch().lap()).count();

            game_engine_module->keyboard_module().flush();

            game_engine_module->rendering_module().on_frame_update.invoke(game_engine_module->rendering());



            graphicsRenderer.Render(&window.Gfx(), &graphicsResources);
            window.Gfx().EndFrame();
        }

        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> Program Sucessfully Ending. See you." << std::flush;
        return exit_code;
    }
    catch (const nodec::NodecException& e)
    {
        nodec::logging::fatal(e.what(), __FILE__, __LINE__);
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
               L"Unhandling Exception has been caught in main loop. \nFor more detail, Please check the 'output.log'.", 
               L"Fatal Error.", MB_OK | MB_ICONEXCLAMATION);
    nodec::logging::warn("[Main] >>> Unexpected Program Ending.", __FILE__, __LINE__);
    return -1;
}
