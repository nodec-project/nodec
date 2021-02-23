#ifndef INPUT__KEYBOARD__IMPL__KEYBOARD_MODULE_HPP_
#define INPUT__KEYBOARD__IMPL__KEYBOARD_MODULE_HPP_

#include <input/keyboard/keyboard.hpp>

#include <queue>
#include <bitset>


namespace input
{
namespace keyboard
{

namespace impl
{
class KeyboardModule : public Keyboard
{
public:
    KeyboardModule() {};

    bool get_key_pressed(Key key) const noexcept override;
    bool get_key_down(Key key) const noexcept override;
    bool get_key_up(Key key) const noexcept override;

public:
    void flush();

public:
    void handle_key_press(Key key);
    void handle_key_release(Key key);
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