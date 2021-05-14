#ifndef INPUT__KEYBOARD__KEYBOARD_HPP_
#define INPUT__KEYBOARD__KEYBOARD_HPP_

#include <input/keyboard/key.hpp>

#include <nodec/macros.hpp>
#include <nodec/signals.hpp>

namespace input {
namespace keyboard {

class Keyboard {
public:
    Keyboard() = default;

public:
    struct Event {
        enum class Type {
            Press,
            Release
        };

        Event(Type type, Key key) :
            type(type),
            key(key) {
        }

        Type type;
        Key key;
    };

public:
    virtual bool get_key_pressed(Key key) const noexcept = 0;
    virtual bool get_key_down(Key key) const noexcept = 0;
    virtual bool get_key_up(Key key) const noexcept = 0;

public:
    using TextInputSignal = nodec::signals::Signal<void(unsigned char)>;
    using KeyboardEventSignal = nodec::signals::Signal<void(const Event&)>;

    virtual TextInputSignal::Interface& on_text_input() = 0;
    virtual KeyboardEventSignal::Interface& on_keyboard_event() = 0;

private:
    NODEC_DISABLE_COPY(Keyboard);
};

}
}

#endif