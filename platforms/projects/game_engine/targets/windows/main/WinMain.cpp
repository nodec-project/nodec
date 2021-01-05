#include "Graphics/GraphicsResources.hpp"
#include "Graphics/RenderingHandlers.hpp"
#include "Graphics/RenderingUtils.hpp"
#include "Graphics/GraphicsRenderer.hpp"
#include "Logging.hpp"
#include "Utils.hpp"
#include "Window.hpp"
#include "ScreenHandlers.hpp"

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
        // --- Init Logging ---
#ifndef NDEBUG
        InitLogging(nodec::logging::Level::Debug);
#else
        InitLogging(nodec::logging::Level::Info);
#endif
        // end Init Logging ---


        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> Hello world. Program start." << std::flush;


        // --- Launch Engine ---
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> launch the Engine." << std::flush;
        auto game_engine_module = nodec::NodecObject::instanciate<nodec_modules::game_engine::GameEngineModule>();
        nodec_modules::game_engine::current = game_engine_module;
        // end Launch Engine ---

        // {
        auto screenHandlers = std::make_shared<ScreenHandlers>();
        ScreenHandlers::SetupOnBootingHandlers(screenHandlers, game_engine_module->screen_module());
        // }

        // {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> boot the Engine." << std::flush;

        if (!nodec_modules::game_engine::boot(game_engine_module.get()))
        {
            throw nodec::NodecException("Engine booting failed.", __FILE__, __LINE__);
        };
        // }

        // {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> launch the window and graphics." << std::flush;
        Window window(game_engine_module->screen_module().size_.x, game_engine_module->screen_module().size_.y,
                      game_engine_module->screen_module().resolution_.x, game_engine_module->screen_module().resolution_.y,
                      L"Nodec Game Engine",
                      &(game_engine_module->keyboard_module()),
                      &(game_engine_module->mouse_module())
        );
        window.SetTitle(game_engine_module->screen_module().title_);
        // }

        // {
        ScreenHandlers::SetupRuntimeHandlers(screenHandlers, &window, game_engine_module->screen_module());
        // }

        GraphicsResources graphicsResources;
        GraphicsRenderer graphicsRenderer(&window.Gfx());

        auto renderingHandlers = std::make_shared<RenderingHandlers>(&window.Gfx(), &graphicsResources, &graphicsRenderer);
        RenderingHandlers::SetupHandlers(renderingHandlers, game_engine_module->rendering_module());


        nodec::logging::InfoStream(__FILE__, __LINE__) 
            << "[Main] >>> [[ START FRAME LOOP ]]\n"
            << "engine_time: " << game_engine_module->engine_time() << " [s]" << std::flush;

        game_engine_module->engine_time_stopwatch().lap();
        int exitCode;
        while (true)
        {
            // process all messages pending, but to not block for new messages
            if (!Window::ProcessMessages(exitCode))
            {
                // if ProcessMessages() returns false, means we're quiting so break
                break;
            }
            window.Gfx().BeginFrame();

            game_engine_module->rendering_module().frame_delta_time_ = std::chrono::duration<float>(game_engine_module->engine_time_stopwatch().lap()).count();

            game_engine_module->keyboard_module().flush();
            game_engine_module->mouse_module().flush();

            game_engine_module->rendering_module().on_frame_update.invoke(game_engine_module->rendering());

            graphicsRenderer.Render(&window.Gfx(), &graphicsResources);

            window.Gfx().EndFrame();
        }

        nodec::logging::InfoStream(__FILE__, __LINE__) 
            << "[Main] >>> [[ END FRAME LOOP ]]\n"
            << "engine_time: " << game_engine_module->engine_time() << " [s]"<< std::flush;

        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> Program Sucessfully Ending. See you." << std::flush;
        return exitCode;
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
               L"Unhandled Exception has been caught in main loop. \nFor more detail, Please check the 'output.log'.",
               L"Fatal Error.", MB_OK | MB_ICONEXCLAMATION);
    nodec::logging::WarnStream(__FILE__, __LINE__) << "[Main] >>> Unexpected Program Ending." << std::flush;
    return -1;
}
