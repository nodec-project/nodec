#ifndef NODEC_INPUT__KEYBOARD__IMPL__KEYBOARD_MODULE_HPP_
#define NODEC_INPUT__KEYBOARD__IMPL__KEYBOARD_MODULE_HPP_

#include "../keyboard.hpp";

#include <queue>
#include <bitset>


namespace nodec_input {
namespace keyboard {

namespace impl {

class KeyboardImpl : public Keyboard {
public:
    KeyboardImpl() = default;

public:
    KeyEventSignal::SignalInterface key_event() { return key_event_.signal_interface(); }
    KeyEventSignal& key_event_impl() { return key_event_; }

private:
    KeyEventSignal key_event_;
};

}
}
}


#endif