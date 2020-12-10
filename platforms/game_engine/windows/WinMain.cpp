#include "Graphics/GraphicsResources.hpp"
#include "Graphics/RenderingHandlers.hpp"
#include "Graphics/RenderingUtils.hpp"
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

void on_boot(nodec_modules::game_engine::interfaces::GameEngine& engine);

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
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> Program start." << std::flush;

        auto game_engine_module = nodec::NodecObject::instanciate<nodec_modules::game_engine::GameEngineModule>();

        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Main] >>> Booting..." << std::flush;

        on_boot(*game_engine_module);

        Window window(1280, 720, L"TEST", game_engine_module.get());
        GraphicsResources graphicsResources;

        auto renderingHandlers = std::make_shared<RenderingHandlers>(&window.Gfx(), &graphicsResources);
        RenderingUtils::InitRenderingHandlers(renderingHandlers, game_engine_module->rendering_module());

        //game_engine_module.keyboard().test = 10;
        //MessageBox(nullptr, std::to_wstring(game_engine_module.keyboard().test).c_str(), L"", MB_OK | MB_ICONEXCLAMATION);
        //game_engine_module.keyboard_module().test = 100;
        //MessageBox(nullptr, std::to_wstring(game_engine_module.keyboard().test).c_str(), L"", MB_OK | MB_ICONEXCLAMATION);

        //window.SetTitle("ほげってる");

        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[Main] >>> Booting finished.\n"
            << "engine_time: " << game_engine_module->engine_time() << "[s]" << std::flush;

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
            game_engine_module->rendering_module().on_frame_update.invoke(game_engine_module->rendering());

            window.Gfx().EndFrame();
        }

        nodec::logging::info("=== Program End ===", __FILE__, __LINE__);
        return exit_code;
    }
    catch (const nodec::NodecException& e)
    {
        nodec::logging::fatal(e.what(), __FILE__, __LINE__);

        try
        {
            auto wideWhat = TryMultiByteToWideChar(e.what());
            auto wideType = TryMultiByteToWideChar(e.type());

            MessageBox(nullptr, wideWhat.c_str(), wideType.c_str(), MB_OK | MB_ICONEXCLAMATION);
        }
        catch (const WideCharacterConvertException& e)
        {
            nodec::logging::error("Wide Character Conversion Error.", __FILE__, __LINE__);
            MessageBox(nullptr, e.what(), e.type(), MB_OK | MB_ICONEXCLAMATION);
        }

    }
    catch (const std::exception& e)
    {

        nodec::logging::fatal(e.what(), __FILE__, __LINE__);

        try
        {
            auto wideWhat = TryMultiByteToWideChar(e.what());

            MessageBox(nullptr, wideWhat.c_str(), L"StandardException", MB_OK | MB_ICONEXCLAMATION);
        }
        catch (const WideCharacterConvertException& e)
        {
            nodec::logging::error("Wide Character Conversion Error.", __FILE__, __LINE__);
            MessageBox(nullptr, e.what(), e.type(), MB_OK | MB_ICONEXCLAMATION);
        }
    }
    catch (...)
    {

        nodec::logging::fatal("Unknown Error Exception Ocuurs.", __FILE__, __LINE__);

        MessageBox(nullptr, L"Unknow Error Occurs. No details available.", L"UnkownErrorException", MB_OK | MB_ICONEXCLAMATION);
    }

    nodec::logging::warn("=== Unexpected Program End ===", __FILE__, __LINE__);
    return -1;
}
