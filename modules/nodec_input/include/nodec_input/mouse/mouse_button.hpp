#ifndef NODEC_INPUT__MOUSE__MOUSE_BUTTON_HPP_
#define NODEC_INPUT__MOUSE__MOUSE_BUTTON_HPP_

#include <nodec/flags.hpp>

#include <sstream>

namespace nodec_input {
namespace mouse {

/**
 * https://doc.qt.io/qt-6/qt.html#MouseButton-enum
 */
enum class MouseButton {
    None = 0x00,
    Left = 0x01 << 0,
    Right = 0x01 << 1,
    Middle = 0x01 << 2,
    _nodec_flags_enable
};

inline std::ostream &operator<<(std::ostream &stream, const MouseButton &button) {
    switch (button) {
    case MouseButton::None: return stream << "None";
    case MouseButton::Left: return stream << "Left";
    case MouseButton::Right: return stream << "Right";
    case MouseButton::Middle: return stream << "Middle";
    default: return stream << "Unknown";
    }
}

using MouseButtons = nodec::Flags<MouseButton>;

} // namespace mouse
} // namespace nodec_input

#endif