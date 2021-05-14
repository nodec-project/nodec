#ifndef INPUT__MOUSE__MOUSE_HPP_
#define INPUT__MOUSE__MOUSE_HPP_

#include <input/mouse/mouse_button.hpp>

#include <nodec/signals.hpp>
#include <nodec/macros.hpp>
#include <nodec/vector2.hpp>

namespace input {
namespace mouse {

class Mouse {
public:
    struct Event {
        enum class Type {
            Press,
            Release,
            Move
        };

        Event(Type type, MouseButton button, nodec::Vector2i position) :
            type(type),
            button(button),
            position(position) {
        }

        Type type;
        MouseButton button;
        nodec::Vector2i position;
    };

public:
    Mouse() = default;

public:
    virtual nodec::Vector2i position() const noexcept = 0;
    virtual bool get_button_down(MouseButton button) = 0;
    virtual bool get_button_up(MouseButton button) = 0;
    virtual bool get_button_pressed(MouseButton button) = 0;

public:
    using MouseEventSignal = nodec::signals::Signal<void(const Event&)>;

    virtual MouseEventSignal::Interface& on_mouse_event() = 0;

private:
    NODEC_DISABLE_COPY(Mouse);
};

}
}

#endif