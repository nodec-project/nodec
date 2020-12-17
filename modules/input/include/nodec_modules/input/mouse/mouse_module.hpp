#ifndef NODEC_MODULES__INPUT__MOUSE__MOUSE_MODULE_HPP_
#define NODEC_MODULES__INPUT__MOUSE__MOUSE_MODULE_HPP_

#include "interfaces/mouse.hpp"

#include <queue>
#include <bitset>

namespace nodec_modules
{
namespace input
{
namespace mouse
{

class MouseModule : public interfaces::Mouse
{
public:
    MouseModule();

public:
    nodec::Vector2i position() const noexcept override;
    bool get_button_down(interfaces::MouseButton button) override;
    bool get_button_up(interfaces::MouseButton button) override;
    bool get_button_pressed(interfaces::MouseButton button) override;

public:
    void flush();

    void handle_mouse_move(nodec::Vector2i position);
    void handle_button_press(interfaces::MouseButton button, nodec::Vector2i position);
    void handle_button_release(interfaces::MouseButton button, nodec::Vector2i position);

private:
    static constexpr unsigned int num_buttons = 32;
    std::queue<Event> event_queue;
    std::bitset<num_buttons> button_states;
    std::bitset<num_buttons> button_states_prev;
    nodec::Vector2i position_;
};

}
}
}

#endif