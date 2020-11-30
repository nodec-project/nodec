#ifndef NODEC_MODULES__INPUT__KEYBOARD_MODULE_HPP_
#define NODEC_MODULES__INPUT__KEYBOARD_MODULE_HPP_

#include <nodec_modules/input/keyboard.hpp>

namespace nodec_modules
{
    namespace input
    {
        class KeyboardModule : public Keyboard
        {
        public:
            void HandleKeyDown();
            void HandleKeyUp();
        };
    }
}

#endif