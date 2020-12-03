#ifndef NODEC_MODULES__INPUT__INTERFACES__KEYBOARD_HPP_
#define NODEC_MODULES__INPUT__INTERFACES__KEYBOARD_HPP_

#include "key.hpp"

#include <nodec/module_interface.hpp>
#include <nodec/event.hpp>

namespace nodec_modules
{
    namespace input
    {
        namespace interfaces
        {

            class Keyboard : public nodec::ModuleInterface
            {
            public:
                using nodec::ModuleInterface::ModuleInterface;

            public:
                bool get_key_pressed() { return true; };
                bool get_key_down();
                bool get_key_up();
                int test;

            public:
                nodec::event::Event<> on_text_input;
                nodec::event::Event<> on_keyboard_event;

            };
        }
    }
}

#endif