#include <nodec/logging.hpp>
#include <nodec_modules/game_engine/game_engine_module.hpp>

namespace nodec_modules
{
namespace game_engine
{

GameEngineModule::GameEngineModule()
    : GameEngine("nodec_modules::game_engine::GameEngineModule")
{
    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[GameEngineModule] >>> Engine strat up..." << std::flush;

    keyboard_module_ = nodec::NodecObject::instanciate<input::keyboard::KeyboardModule>();

    keyboard_module_ = nodec::NodecObject::instanciate<input::keyboard::KeyboardModule>();

    mouse_module_ = nodec::NodecObject::instanciate<input::mouse::MouseModule>();

    rendering_module_ = nodec::NodecObject::instanciate<rendering::RenderingModule>();

    screen_module_ = nodec::NodecObject::instanciate<screen::ScreenModule>();
    screen_module_->size_.set(1920, 1080);
    screen_module_->resolution_.set(1920, 1080);
    screen_module_->title_ = "Nodec Game Engine";

    root_scene_object_ = nodec::NodecObject::instanciate<nodec::scene_set::SceneObject>(
        "root_scene_object");

    engine_time_stopwatch_.reset();
    engine_time_stopwatch_.start();

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[GameEngineModule] >>> Sucessfully start up.\n"
        << "engine_time: " << engine_time() << "[s]" << std::flush;
}

GameEngineModule::~GameEngineModule()
{

    //root_scene_object_.reset();

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[GameEngineModule] >>> Engine now shuting down.\n"
        << "engine_time: " << engine_time() << "[s]" << std::flush;


}


// === interface ===
input::keyboard::interfaces::Keyboard&
GameEngineModule::keyboard() const noexcept
{
    return *keyboard_module_;
}

input::mouse::interfaces::Mouse&
GameEngineModule::mouse() const noexcept
{
    return *mouse_module_;
}

rendering::interfaces::Rendering&
GameEngineModule::rendering() const noexcept
{
    return *rendering_module_;
}

screen::interfaces::Screen&
GameEngineModule::screen() const noexcept
{
    return *screen_module_;
}

float
GameEngineModule::engine_time() const noexcept
{
    return std::chrono::duration<float>(engine_time_stopwatch_.elapsed()).count();
}

nodec::scene_set::SceneObject&
GameEngineModule::root_scene_object() const noexcept
{
    return *root_scene_object_;
}

// End interface ===

input::keyboard::KeyboardModule&
GameEngineModule::keyboard_module() const noexcept
{
    return (*keyboard_module_);
}

input::mouse::MouseModule&
GameEngineModule::mouse_module() const noexcept
{
    return (*mouse_module_);
}

rendering::RenderingModule&
GameEngineModule::rendering_module() const noexcept
{
    return (*rendering_module_);
}

screen::ScreenModule&
GameEngineModule::screen_module() const noexcept
{
    return (*screen_module_);
}

nodec::Stopwatch<std::chrono::steady_clock>&
GameEngineModule::engine_time_stopwatch() noexcept
{
    return (engine_time_stopwatch_);
}

static nodec::NodecObject::Holder<GameEngineModule> game_engine_module;

GameEngineModule* get_game_engine_module()
{
    if (!game_engine_module)
    {
        game_engine_module = nodec::NodecObject::instanciate<GameEngineModule>();
    }
    return game_engine_module.get();
}

bool boot(GameEngineModule* game_engine_module) noexcept
{
    nodec::logging::InfoStream(__FILE__, __LINE__) << "[GameEngineModule] >>> Booting..." << std::flush;
    bool success = false;
    try
    {
        interfaces::on_boot(*game_engine_module);
        success = true;
    }
    catch (const nodec::NodecException& e)
    {
        nodec::logging::ErrorStream(__FILE__, __LINE__) 
            << "[GameEngineModule] >>> A NodecException has been occured while booting.\n"
            << "detail: " << e.what() << std::flush;
    }
    catch (const std::exception& e)
    {
        nodec::logging::ErrorStream(__FILE__, __LINE__) 
            << "[GameEngineModule] >>> A StandardException has been occured while booting.\n"
            << "detail: " << e.what() << std::flush;
    }
    catch (...)
    {
        nodec::logging::ErrorStream(__FILE__, __LINE__) 
            << "[GameEngineModule] >>> An UnknownException has occured while booting.\n"
            << "detail: Unavailable." << std::flush;
    }

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[GameEngineModule] >>> Booting finished.\n"
        << "success: " << success << "\n"
        << "engine_time: " << game_engine_module->engine_time() << "[s]" << std::flush;

    return success;
}

namespace interfaces
{

GameEngine* get_engine() noexcept
{
    return game_engine_module.get();
}

} // namespace interfaces


} // namespace game_engine
} // namespace nodec_modules