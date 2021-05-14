#ifndef INPUT__KEYBOARD__IMPL__KEYBOARD_MODULE_HPP_
#define INPUT__KEYBOARD__IMPL__KEYBOARD_MODULE_HPP_

#include <input/keyboard/keyboard.hpp>

#include <queue>
#include <bitset>


namespace input {
namespace keyboard {

namespace impl {
class KeyboardModule : public Keyboard {
public:
    KeyboardModule() = default;

    bool get_key_pressed(Key key) const noexcept override;
    bool get_key_down(Key key) const noexcept override;
    bool get_key_up(Key key) const noexcept override;


    TextInputSignal::Interface& on_text_input() override {
        return on_text_input_;
    }

    KeyboardEventSignal::Interface& on_keyboard_event() override {
        return on_keyboard_event_;
    }

public:
    void flush();

public:
    void handle_key_press(Key key);
    void handle_key_release(Key key);
    void handle_text_input(unsigned char text);

private:
    TextInputSignal on_text_input_;
    KeyboardEventSignal on_keyboard_event_;

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