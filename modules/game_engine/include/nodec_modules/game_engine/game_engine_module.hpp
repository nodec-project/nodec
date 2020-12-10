#ifndef NODEC_MODULES__GAME_ENGINE__GAME_ENGINE_MODULE_HPP_
#define NODEC_MODULES__GAME_ENGINE__GAME_ENGINE_MODULE_HPP_

#include "interfaces/game_engine.hpp"

#include <nodec_modules/input/keyboard_module.hpp>
#include <nodec_modules/input/mouse_module.hpp>
#include <nodec_modules/rendering/rendering_module.hpp>

#include <string>

namespace nodec_modules
{
namespace game_engine
{
class GameEngineModule : public interfaces::GameEngine
{
public:
    GameEngineModule();
    ~GameEngineModule();

public:
    input::interfaces::Keyboard&
        keyboard() const noexcept override;

    input::interfaces::Mouse&
        mouse() const noexcept override;

    rendering::interfaces::Rendering&
        rendering() const noexcept override;

    nodec::scene_set::SceneObject&
        root_scene_object() const noexcept override;

    float engine_time() const noexcept override;

public:
    input::KeyboardModule&
        keyboard_module() const noexcept;

    input::MouseModule&
        mouse_module() const noexcept;

    rendering::RenderingModule&
        rendering_module() const noexcept;

    nodec::Stopwatch<std::chrono::steady_clock>&
        engine_time_stopwatch() noexcept;

protected:
    nodec::NodecObject::Holder<input::KeyboardModule> keyboard_module_;
    nodec::NodecObject::Holder<input::MouseModule> mouse_module_;
    nodec::NodecObject::Holder<rendering::RenderingModule> rendering_module_;

    nodec::NodecObject::Holder<nodec::scene_set::SceneObject> root_scene_object_;

    nodec::Stopwatch<std::chrono::steady_clock> engine_time_stopwatch_;
};

GameEngineModule* get_game_engine_module();

} // namespace game_engine
} // namespace nodec_modules

#endif