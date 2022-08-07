#ifndef NODEC_INPUT__MOUSE__MOUSE_BUTTON_HPP_
#define NODEC_INPUT__MOUSE__MOUSE_BUTTON_HPP_

#include <nodec/flags/allow_flags.hpp>
#include <nodec/flags/flags.hpp>

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
    Middle = 0x01 << 2
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

} // namespace mouse
} // namespace nodec_input

NODEC_ALLOW_FLAGS_FOR_ENUM(nodec_input::mouse::MouseButton);

namespace nodec_input {
namespace mouse {

using MouseButtons = nodec::flags::Flags<MouseButton>;

}
} // namespace nodec_input

#endif