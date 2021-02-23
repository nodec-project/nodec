#ifndef GAME_ENGINE__GAME_ENGINE_HPP_
#define GAME_ENGINE__GAME_ENGINE_HPP_


#include <input/keyboard/keyboard.hpp>
#include <input/mouse/mouse.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>
#include <nodec_modules/screen/interfaces/screen.hpp>


#include <nodec/scene_set/scene_object.hpp>
#include <nodec/stopwatch.hpp>


namespace game_engine
{

class NoEngineException : public nodec::NodecException
{
public:
    using NodecException::NodecException;
};

class GameEngine
{
private:
    NODEC_DISABLE_COPY(GameEngine);

public:
    GameEngine() {};

public:
    virtual nodec::scene_set::SceneObject& root_scene_object() const noexcept = 0;

    virtual input::keyboard::Keyboard& keyboard() const noexcept = 0;

    virtual input::mouse::Mouse& mouse() const noexcept = 0;

    virtual nodec_modules::rendering::interfaces::Rendering& rendering() const noexcept = 0;

    virtual nodec_modules::screen::interfaces::Screen& screen() const noexcept = 0;

    virtual float engine_time() const noexcept = 0;
};

void on_boot(GameEngine& engine);

GameEngine* get_engine();

}  // namespace game_engine

#endif