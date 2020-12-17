#ifndef NODEC_MODULES__INPUT__KEYBOARD__KEYBOARD_MODULE_HPP_
#define NODEC_MODULES__INPUT__KEYBOARD__KEYBOARD_MODULE_HPP_

#include "interfaces/keyboard.hpp"

#include <queue>
#include <bitset>

namespace nodec_modules
{
namespace input
{
namespace keyboard
{

class KeyboardModule : public interfaces::Keyboard
{
public:
    KeyboardModule();

    bool get_key_pressed(interfaces::Key key) const noexcept override;
    bool get_key_down(interfaces::Key key) const noexcept override;
    bool get_key_up(interfaces::Key key) const noexcept override;

public:
    void flush();

public:
    void handle_key_press(interfaces::Key key);
    void handle_key_release(interfaces::Key key);
    void handle_text_input(unsigned char text);

private:
    static constexpr unsigned int num_keys = 256u;
    std::bitset<num_keys> key_states;
    std::bitset<num_keys> key_states_prev;
    std::queue<Event> event_queue;
    std::queue<unsigned char> text_queue;
};

}
}
}

#endif