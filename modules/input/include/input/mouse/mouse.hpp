#ifndef INPUT__MOUSE__MOUSE_HPP_
#define INPUT__MOUSE__MOUSE_HPP_

#include <input/mouse/mouse_button.hpp>

#include <nodec/module_interface.hpp>
#include <nodec/event.hpp>
#include <nodec/vector2.hpp>

namespace input
{
namespace mouse
{

class Mouse : public nodec::ModuleInterface
{
public:
    using ModuleInterface::ModuleInterface;

public:
    struct Event
    {
        enum class Type
        {
            Press,
            Release,
            Move
        };

        Event(Type type, MouseButton button, nodec::Vector2i position) :
            type(type),
            button(button),
            position(position)
        {
        }

        Type type;
        MouseButton button;
        nodec::Vector2i position;

    };

public:
    virtual nodec::Vector2i position() const noexcept = 0;
    virtual bool get_button_down(MouseButton button) = 0;
    virtual bool get_button_up(MouseButton button) = 0;
    virtual bool get_button_pressed(MouseButton button) = 0;

public:
    nodec::event::Event<const Event&> on_mouse_event;

};

} 
}

#endif