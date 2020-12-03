#ifndef NODEC_MODULES__INPUT__KEYBOARD_MODULE_HPP_
#define NODEC_MODULES__INPUT__KEYBOARD_MODULE_HPP_

#include "interfaces/keyboard.hpp"

#include <bitset>

namespace nodec_modules
{
    namespace input
    {
        class KeyboardModule : public interfaces::Keyboard
        {
        public:
            KeyboardModule();

        public:
            void flush();

        public:
            void handle_key_down();
            void handle_key_up();
            void handle_text_input();

        private:
            static constexpr unsigned int num_keys = 256;
            std::bitset<num_keys> key_states;
            std::bitset<num_keys> key_states_prev;
        };
    }
}

#endif