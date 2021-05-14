#ifndef GAME_ENGINE__IMPL__GAME_ENGINE_MODULE_HPP_
#define GAME_ENGINE__IMPL__GAME_ENGINE_MODULE_HPP_

#include <game_engine/game_engine.hpp>
#include <input/keyboard/impl/keyboard_module.hpp>
#include <input/mouse/impl/mouse_module.hpp>

#include <nodec/object.hpp>
#include <screen/impl/screen_module.hpp>
#include <string>

namespace game_engine {
namespace impl {

class GameEngineModule : public GameEngine {
public:
    GameEngineModule();
    ~GameEngineModule();

public:
    scene_set::SceneRegistry& scene_registry() noexcept override {
        return scene_registry_;
    }

    input::keyboard::Keyboard& keyboard() noexcept override {
        return keyboard_module_;
    };

    input::mouse::Mouse& mouse() noexcept override {
        return mouse_module_;
    };

    screen::Screen& screen() noexcept override {
        return screen_module_;
    };

    float engine_time() const noexcept override {
        return std::chrono::duration<float>(engine_time_stopwatch_.elapsed()).count();
    }

public:
    input::keyboard::impl::KeyboardModule& keyboard_module() noexcept {
        return keyboard_module_;
    };

    input::mouse::impl::MouseModule& mouse_module() noexcept {
        return mouse_module_;
    }

    screen::impl::ScreenModule& screen_module() noexcept {
        return screen_module_;
    }

    nodec::Stopwatch<std::chrono::steady_clock>& engine_time_stopwatch() noexcept {
        return engine_time_stopwatch_;
    }

private:
    scene_set::SceneRegistry scene_registry_;

    input::keyboard::impl::KeyboardModule keyboard_module_;

    input::mouse::impl::MouseModule mouse_module_;

    screen::impl::ScreenModule screen_module_;

    nodec::Stopwatch<std::chrono::steady_clock> engine_time_stopwatch_;
};

void set_current(GameEngineModule* engine);
bool boot(GameEngineModule& game_engine_module) noexcept;

}  // namespace impl
}  // namespace game_engine

#endif