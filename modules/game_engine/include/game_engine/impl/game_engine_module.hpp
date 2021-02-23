#ifndef GAME_ENGINE__IMPL__GAME_ENGINE_MODULE_HPP_
#define GAME_ENGINE__IMPL__GAME_ENGINE_MODULE_HPP_

#include <game_engine/game_engine.hpp>
#include <input/keyboard/impl/keyboard_module.hpp>
#include <input/mouse/impl/mouse_module.hpp>
#include <nodec_modules/rendering/rendering_module.hpp>
#include <nodec_modules/screen/screen_module.hpp>

#include <nodec/object.hpp>

#include <string>


namespace game_engine
{
namespace impl
{

class GameEngineModule : public GameEngine
{
public:
    GameEngineModule();
    ~GameEngineModule();

public:
    input::keyboard::Keyboard&
        keyboard() const noexcept override;

    input::mouse::Mouse&
        mouse() const noexcept override;

    nodec_modules::rendering::interfaces::Rendering&
        rendering() const noexcept override;

    nodec_modules::screen::interfaces::Screen&
        screen() const noexcept override;

    nodec::scene_set::SceneObject&
        root_scene_object() const noexcept override;

    float engine_time() const noexcept override;

public:
    input::keyboard::impl::KeyboardModule&
        keyboard_module() const noexcept;

    input::mouse::impl::MouseModule&
        mouse_module() const noexcept;

    nodec_modules::rendering::RenderingModule&
        rendering_module() const noexcept;

    nodec_modules::screen::ScreenModule&
        screen_module() const noexcept;

    nodec::Stopwatch<std::chrono::steady_clock>&
        engine_time_stopwatch() noexcept;


protected:
    nodec::Object::Holder<input::keyboard::impl::KeyboardModule> keyboard_module_;
    nodec::Object::Holder<input::mouse::impl::MouseModule> mouse_module_;


    nodec::NodecObject::Holder<nodec_modules::rendering::RenderingModule> rendering_module_;
    nodec::NodecObject::Holder<nodec_modules::screen::ScreenModule> screen_module_;

    nodec::NodecObject::Holder<nodec::scene_set::SceneObject> root_scene_object_;

    nodec::Stopwatch<std::chrono::steady_clock> engine_time_stopwatch_;
};

extern nodec::NodecObject::Reference<GameEngineModule> current;

bool boot(GameEngineModule* game_engine_module) noexcept;

}


} // namespace game_engine

#endif