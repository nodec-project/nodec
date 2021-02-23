/*
* 
*/
#include <game_engine/impl/game_engine_module.hpp>
#include <nodec/logging.hpp>

using namespace nodec_modules;

namespace game_engine
{
namespace impl
{

GameEngineModule::GameEngineModule()
{
    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[GameEngineModule] >>> Engine strat up..." << std::flush;

    keyboard_module_ = nodec::Object::make_holder<input::keyboard::impl::KeyboardModule>();

    mouse_module_ = nodec::Object::make_holder<input::mouse::impl::MouseModule>();

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
input::keyboard::Keyboard&
GameEngineModule::keyboard() const noexcept
{
    return *keyboard_module_;
}

input::mouse::Mouse&
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

input::keyboard::impl::KeyboardModule&
GameEngineModule::keyboard_module() const noexcept
{
    return (*keyboard_module_);
}

input::mouse::impl::MouseModule&
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

nodec::NodecObject::Reference<GameEngineModule> current;


bool boot(GameEngineModule* game_engine_module) noexcept
{
    nodec::logging::InfoStream(__FILE__, __LINE__) << "[GameEngineModule] >>> Booting..." << std::flush;
    bool success = false;
    try
    {
        on_boot(*game_engine_module);
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
            << "[GameEngineModule] >>> An UnknownException has been occured while booting.\n"
            << "detail: Unavailable." << std::flush;
    }

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[GameEngineModule] >>> Booting finished.\n"
        << "success: " << success << "\n"
        << "engine_time: " << game_engine_module->engine_time() << "[s]" << std::flush;

    return success;
}

}

GameEngine* get_engine()
{
    if (auto locked = impl::current.lock())
    {
        return locked.get();
    }
    throw NoEngineException("Current Engine not assigned. May be not instanciated or already deleted.", __FILE__, __LINE__);
}



} // namespace game_engine