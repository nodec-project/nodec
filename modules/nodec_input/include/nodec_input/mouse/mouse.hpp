#ifndef NODEC_INPUT__MOUSE__MOUSE_HPP_
#define NODEC_INPUT__MOUSE__MOUSE_HPP_

#include "mouse_button.hpp"

#include <nodec/macros.hpp>
#include <nodec/signals.hpp>
#include <nodec/vector2.hpp>
#include <nodec/logging.hpp>

namespace nodec_input {
namespace mouse {

struct MouseEvent {
    enum class Type {
        Press,
        Release,
        Move
    };

    MouseEvent(Type type, MouseButton button, MouseButtons buttons, nodec::Vector2i position)
        : type(type),
          button(button),
          buttons(buttons),
          position(position) {
    }

    Type type;
    MouseButton button;
    MouseButtons buttons;
    nodec::Vector2i position;

    //friend std::ostream &operator<<(std::ostream &stream, const nodec_input::mouse::MouseEvent &mouseEvent) {
    //    return stream << "{ type: " << mouseEvent.type << ", button: " << mouseEvent.button
    //                  << ", buttons: " << mouseEvent.buttons
    //                  << ", position: " << mouseEvent.position << " }";
    //}
};

// NOTE: Should define << operator in the same namespace with right hand operand.
//  Otherwise, if the left operand is a class inherited from ostream, 
//  the compiler does not seem to find the << operator.

inline std::ostream &operator<<(std::ostream &stream, const nodec_input::mouse::MouseEvent::Type &type) {
    using namespace nodec_input::mouse;

    switch (type) {
    case MouseEvent::Type::Press: return stream << "Press";
    case MouseEvent::Type::Release: return stream << "Release";
    case MouseEvent::Type::Move: return stream << "Move";
    default: return stream << "Unknown";
    }
}

inline std::ostream &operator<<(std::ostream &stream, const nodec_input::mouse::MouseEvent &mouseEvent) {
    return stream << "{ type: " << mouseEvent.type << ", button: " << mouseEvent.button
                  << ", buttons: " << mouseEvent.buttons
                  << ", position: " << mouseEvent.position << " }";
}


class Mouse {
public:
    using MouseEventSignal = nodec::signals::Signal<void(const MouseEvent &)>;

public:
    Mouse() = default;

public:
    virtual MouseEventSignal::SignalInterface mouse_event() = 0;

private:
    NODEC_DISABLE_COPY(Mouse)
};

} // namespace mouse
} // namespace nodec_input

#endif