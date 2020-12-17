#include <nodec_modules/input/mouse/mouse_module.hpp>

namespace nodec_modules
{
namespace input
{
namespace mouse
{

MouseModule::MouseModule() :
    Mouse("nodec_modules::input::MouseModule")
{

}

nodec::Vector2i MouseModule::position() const noexcept
{
    return position_;
}

bool MouseModule::get_button_down(interfaces::MouseButton button)
{
    auto code = static_cast<int>(button);
    return button_states[code] && (button_states[code] != button_states_prev[code]);
}
bool MouseModule::get_button_up(interfaces::MouseButton button)
{
    auto code = static_cast<int>(button);
    return !button_states[code] && (button_states[code] != button_states_prev[code]);
}
bool MouseModule::get_button_pressed(interfaces::MouseButton button)
{
    return button_states[static_cast<int>(button)];
}

void MouseModule::flush()
{
    button_states_prev = button_states;

    while (!event_queue.empty())
    {
        Event& mouse_event = event_queue.front();
        switch (mouse_event.type)
        {
        case Event::Type::Move:
            position_.set(mouse_event.position.x, mouse_event.position.y);
            break;
        case Event::Type::Press:
            position_.set(mouse_event.position.x, mouse_event.position.y);
            button_states[static_cast<int>(mouse_event.button)] = true;
            break;

        case Event::Type::Release:
            position_.set(mouse_event.position.x, mouse_event.position.y);
            button_states[static_cast<int>(mouse_event.button)] = false;
            break;
        }
        on_mouse_event.invoke(mouse_event);
        event_queue.pop();
    }
}


void MouseModule::handle_mouse_move(nodec::Vector2i position)
{
    event_queue.push({ Event::Type::Move, interfaces::MouseButton::None, position });
}

void MouseModule::handle_button_press(interfaces::MouseButton button, nodec::Vector2i position)
{
    event_queue.push({ Event::Type::Press, button, position });
}

void MouseModule::handle_button_release(interfaces::MouseButton button, nodec::Vector2i position)
{
    event_queue.push({ Event::Type::Release, button, position });
}

} // namespace mouse
} // namespace input
} // namespace nodec_modules