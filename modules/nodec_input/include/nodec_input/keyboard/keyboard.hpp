#ifndef NODEC_INPUT__KEYBOARD__KEYBOARD_HPP_
#define NODEC_INPUT__KEYBOARD__KEYBOARD_HPP_

#include "key.hpp"

#include <nodec/macros.hpp>
#include <nodec/signals.hpp>

#include <sstream>

namespace nodec_input {
namespace keyboard {

struct KeyEvent {
    enum class Type {
        Press,
        Release
    };

    KeyEvent(Type type, Key key)
        : type(type), key(key) {
    }

    Type type;
    Key key;
};

inline std::ostream &operator<<(std::ostream &stream, const KeyEvent::Type &type) {
    switch (type) {
    case KeyEvent::Type::Press: return stream << "Press";
    case KeyEvent::Type::Release: return stream << "Release";
    default: return stream << "Unknown";
    }
}

inline std::ostream &operator<<(std::ostream &stream, const KeyEvent &keyEvent) {
    return stream << "{ type: " << keyEvent.type << ", key: " << keyEvent.key << " )";
}

class Keyboard {
public:
    using KeyEventSignal = nodec::signals::Signal<void(const KeyEvent &)>;

public:
    Keyboard() = default;

public:
    virtual KeyEventSignal::SignalInterface key_event() = 0;

private:
    NODEC_DISABLE_COPY(Keyboard);
};

} // namespace keyboard
} // namespace nodec_input

#endif