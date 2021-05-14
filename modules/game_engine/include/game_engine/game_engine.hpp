#ifndef GAME_ENGINE__GAME_ENGINE_HPP_
#define GAME_ENGINE__GAME_ENGINE_HPP_

#include <input/keyboard/keyboard.hpp>
#include <input/mouse/mouse.hpp>
#include <scene_set/scene_registry.hpp>
#include <screen/screen.hpp>

#include <nodec/stopwatch.hpp>

namespace game_engine {

class GameEngine {
public:
    GameEngine() = default;

    virtual scene_set::SceneRegistry& scene_registry() noexcept = 0;

    virtual input::keyboard::Keyboard& keyboard() noexcept = 0;

    virtual input::mouse::Mouse& mouse() noexcept = 0;

    virtual screen::Screen& screen() noexcept = 0;

    virtual float engine_time() const noexcept = 0;

private:
    NODEC_DISABLE_COPY(GameEngine);
};

void on_boot(GameEngine& engine);

GameEngine& current();

} // namespace game_engine

#endif