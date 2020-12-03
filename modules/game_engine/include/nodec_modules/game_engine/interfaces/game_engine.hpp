#ifndef NODEC_MODULES__GAME_ENGINE__INTERFACES__GAME_ENGINE_HPP_
#define NODEC_MODULES__GAME_ENGINE__INTERFACES__GAME_ENGINE_HPP_

#include <nodec_modules/input/interfaces/keyboard.hpp>
#include <nodec_modules/input/interfaces/mouse.hpp>

#include <nodec/module_interface.hpp>

#include <string>

namespace nodec_modules
{
    namespace game_engine
    {
        namespace interfaces
        {
            class GameEngine : public nodec::ModuleInterface
            {
            public:
                using nodec::ModuleInterface::ModuleInterface;

            public:
                virtual input::interfaces::Keyboard& keyboard() const noexcept = 0;
                virtual input::interfaces::Mouse& mouse() const noexcept = 0;

            };
        }
    }
}

#endif