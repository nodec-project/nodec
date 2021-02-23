#include "Application.hpp"

#include "Graphics/GraphicsResources.hpp"
#include "Graphics/RenderingHandlers.hpp"
#include "Graphics/RenderingUtils.hpp"
#include "Graphics/GraphicsRenderer.hpp"
#include "Logging.hpp"
#include "Utils.hpp"
#include "Window.hpp"
#include "ScreenHandlers.hpp"


#include <game_engine/impl/game_engine_module.hpp>

#include <nodec/logging.hpp>
#include <nodec/nodec_exception.hpp>
#include <nodec/unicode.hpp>


int Application::main_impl()
{

    // === launch the engine ===
    nodec::logging::InfoStream(__FILE__, __LINE__) 
        << "[Main] >>> launch the Engine" << std::flush;

    auto gameEngineModule = nodec::Object::make_holder<game_engine::impl::GameEngineModule>();
    game_engine::impl::current = gameEngineModule;
    // End launch the engine ===


    auto screenHandlers = std::make_shared<ScreenHandlers>();
    ScreenHandlers::SetupOnBootingHandlers(screenHandlers, gameEngineModule->screen_module());
    

    // === Engine booting ===
    nodec::logging::InfoStream(__FILE__, __LINE__) 
        << "[Main] >>> boot the Engine" << std::flush;


    if (!game_engine::impl::boot(gameEngineModule.get()))
    {
        throw nodec::NodecException("Engine booting failed", __FILE__, __LINE__);
    };
    // End Engine Booting ===
    

    // === launch Window ===
    nodec::logging::InfoStream(__FILE__, __LINE__) 
        << "[Main] >>> launch the window" << std::flush;
    Window window(gameEngineModule->screen_module().size_.x, gameEngineModule->screen_module().size_.y,
                  gameEngineModule->screen_module().resolution_.x, gameEngineModule->screen_module().resolution_.y,
                  L"Nodec Game Engine",
                  &(gameEngineModule->keyboard_module()),
                  &(gameEngineModule->mouse_module())
    );
    window.SetTitle(gameEngineModule->screen_module().title_);
    // End launch Window ===
    

    ScreenHandlers::SetupRuntimeHandlers(screenHandlers, &window, gameEngineModule->screen_module());
    

    GraphicsResources graphicsResources;
    GraphicsRenderer graphicsRenderer(&window.Gfx());

    auto renderingHandlers = std::make_shared<RenderingHandlers>(&window.Gfx(), &graphicsResources, &graphicsRenderer);
    RenderingHandlers::SetupHandlers(renderingHandlers, gameEngineModule->rendering_module());


    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[Main] >>> [[ START FRAME LOOP ]]\n"
        << "engine_time: " << gameEngineModule->engine_time() << " [s]" << std::flush;

    gameEngineModule->engine_time_stopwatch().lap();
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

        gameEngineModule->rendering_module().frame_delta_time_ = std::chrono::duration<float>(gameEngineModule->engine_time_stopwatch().lap()).count();

        gameEngineModule->keyboard_module().flush();
        gameEngineModule->mouse_module().flush();

        gameEngineModule->rendering_module().on_frame_update.invoke(gameEngineModule->rendering());

        graphicsRenderer.Render(&window.Gfx(), &graphicsResources);

        window.Gfx().EndFrame();
    }

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[Main] >>> [[ END FRAME LOOP ]]\n"
        << "engine_time: " << gameEngineModule->engine_time() << " [s]" << std::flush;

    return exitCode;
}