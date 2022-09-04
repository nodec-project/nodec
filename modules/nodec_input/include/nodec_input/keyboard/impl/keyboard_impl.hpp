#ifndef NODEC_INPUT__KEYBOARD__IMPL__KEYBOARD_MODULE_HPP_
#define NODEC_INPUT__KEYBOARD__IMPL__KEYBOARD_MODULE_HPP_

#include "../keyboard.hpp"

#include <bitset>
#include <queue>

namespace nodec_input {
namespace keyboard {

namespace impl {

class KeyboardImpl : public Keyboard {
public:
    KeyboardImpl() = default;

public:
    KeyEventSignal::SignalInterface key_event() {
        return key_event_.signal_interface();
    }

public:
    void handle_key_event(const KeyEvent::Type &type, const Key &key) {
        key_event_({type, key});
    }

private:
    KeyEventSignal key_event_;
};

} // namespace impl
} // namespace keyboard
} // namespace nodec_input

#endif