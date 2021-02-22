#ifndef NODEC_MODULES__GAME_ENGINE__INTERFACES__GAME_ENGINE_HPP_
#define NODEC_MODULES__GAME_ENGINE__INTERFACES__GAME_ENGINE_HPP_

#include <input/keyboard/keyboard.hpp>
#include <input/mouse/mouse.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>
#include <nodec_modules/screen/interfaces/screen.hpp>

#include <nodec/module_interface.hpp>
#include <nodec/scene_set/scene_object.hpp>
#include <nodec/stopwatch.hpp>



namespace nodec_modules
{
namespace game_engine
{
namespace interfaces
{

class NoEngineException : public nodec::NodecException
{
public:
    using NodecException::NodecException;
};

class GameEngine : public nodec::ModuleInterface
{
public:
    using nodec::ModuleInterface::ModuleInterface;

public:
    virtual nodec::scene_set::SceneObject& root_scene_object() const noexcept = 0;

    virtual input::keyboard::Keyboard& keyboard() const noexcept = 0;

    virtual input::mouse::Mouse& mouse() const noexcept = 0;

    virtual rendering::interfaces::Rendering& rendering() const noexcept = 0;

    virtual screen::interfaces::Screen& screen() const noexcept = 0;

    virtual float engine_time() const noexcept = 0;
};

void on_boot(GameEngine& engine);

GameEngine* get_engine();

}  // namespace interfaces
}  // namespace game_engine
}  // namespace nodec_modules

#endif