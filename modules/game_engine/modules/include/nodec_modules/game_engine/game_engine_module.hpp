#ifndef NODEC_MODULES__GAME_ENGINE__GAME_ENGINE_MODULE_HPP_
#define NODEC_MODULES__GAME_ENGINE__GAME_ENGINE_MODULE_HPP_

#include <nodec_modules/game_engine/game_engine.hpp>

#include <nodec_modules/input/keyboard_module.hpp>
#include <nodec_modules/input/mouse_module.hpp>

#include <string>

namespace nodec_modules
{
    namespace game_engine
    {
        class GameEngineModule : public GameEngine
        {
        public:
            GameEngineModule();

        public:
            input::KeyboardModule& keyboard_module() const noexcept;
            input::MouseModule& mouse_module() const noexcept;

        protected:
            nodec::NodecObjectHolder<input::KeyboardModule> keyboard_module_;
            nodec::NodecObjectHolder<input::MouseModule> mouse_module_;
        };
    }
}

#endif