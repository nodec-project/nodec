#ifndef INPUT__MOUSE__IMPL__MOUSE_MODULE_HPP_
#define INPUT__MOUSE__IMPL__MOUSE_MODULE_HPP_

#include <input/mouse/mouse.hpp>

#include <queue>
#include <bitset>


namespace input
{
namespace mouse
{
namespace impl
{

class MouseModule : public Mouse
{
public:
    MouseModule();

public:
    nodec::Vector2i position() const noexcept override;
    bool get_button_down(MouseButton button) override;
    bool get_button_up(MouseButton button) override;
    bool get_button_pressed(MouseButton button) override;

public:
    void flush();

    void handle_mouse_move(nodec::Vector2i position);
    void handle_button_press(MouseButton button, nodec::Vector2i position);
    void handle_button_release(MouseButton button, nodec::Vector2i position);

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