#ifndef NODEC_INPUT__MOUSE__IMPL__MOUSE_MODULE_HPP_
#define NODEC_INPUT__MOUSE__IMPL__MOUSE_MODULE_HPP_

#include "../mouse.hpp"

namespace nodec_input {
namespace mouse {
namespace impl {

class MouseDevice : public Mouse {
public:
    MouseDevice() = default;

public:
    MouseEventSignal::SignalInterface mouse_event() override {
        return mouse_event_.signal_interface();
    }

public:
    void handle_mouse_move_event(const MouseButtons &buttons, const nodec::Vector2i &position) {
        mouse_event_({MouseEvent::Type::Move, MouseButton::None, buttons, position});
    }

    void handle_mouse_press_event(const MouseButton &button, const MouseButtons &buttons, const nodec::Vector2i &position) {
        mouse_event_({MouseEvent::Type::Press, button, buttons, position});
    }

    void handle_mouse_release_event(const MouseButton &button, const MouseButtons &buttons, const nodec::Vector2i &position) {
        mouse_event_({MouseEvent::Type::Release, button, buttons, position});
    }

private:
    MouseEventSignal mouse_event_;
};

} // namespace impl
} // namespace mouse
} // namespace nodec_input

#endif