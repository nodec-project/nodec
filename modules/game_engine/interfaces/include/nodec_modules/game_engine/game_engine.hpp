#ifndef NODEC_MODULES__GAME_ENGINE__GAME_ENGINE_HPP_
#define NODEC_MODULES__GAME_ENGINE__GAME_ENGINE_HPP_

#include <nodec_modules/input/keyboard.hpp>
#include <nodec_modules/input/mouse.hpp>

#include <nodec/module_interface.hpp>

#include <string>

namespace nodec_modules
{
    namespace game_engine
    {

        class GameEngine : public nodec::ModuleInterface
        {
        public:
            using nodec::ModuleInterface::ModuleInterface;

        public:
            input::Keyboard& keyboard() const noexcept;
            input::Mouse& mouse() const noexcept;

        protected:
            nodec::NodecObjectHolder<input::Keyboard> keyboard_;
            nodec::NodecObjectHolder<input::Mouse> mouse_;

        };
    }
}

#endif