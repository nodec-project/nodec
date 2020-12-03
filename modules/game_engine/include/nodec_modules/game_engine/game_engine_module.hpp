#ifndef NODEC_MODULES__GAME_ENGINE__GAME_ENGINE_MODULE_HPP_
#define NODEC_MODULES__GAME_ENGINE__GAME_ENGINE_MODULE_HPP_

#include "interfaces/game_engine.hpp"

#include <nodec_modules/input/keyboard_module.hpp>
#include <nodec_modules/input/mouse_module.hpp>

#include <string>

namespace nodec_modules
{
    namespace game_engine
    {
        class GameEngineModule : public interfaces::GameEngine
        {
        public:
            GameEngineModule();

        public:
            input::interfaces::Keyboard& keyboard() const noexcept override;
            input::interfaces::Mouse& mouse() const noexcept override;

            input::KeyboardModule& keyboard_module() const noexcept;
            input::MouseModule& mouse_module() const noexcept;

        protected:
            nodec::NodecObjectHolder<input::KeyboardModule> keyboard_module_;
            nodec::NodecObjectHolder<input::MouseModule> mouse_module_;
        };
    }
}

#endif