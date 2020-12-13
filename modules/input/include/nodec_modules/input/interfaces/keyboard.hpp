#ifndef NODEC_MODULES__INPUT__INTERFACES__KEYBOARD_HPP_
#define NODEC_MODULES__INPUT__INTERFACES__KEYBOARD_HPP_

#include "key.hpp"

#include <nodec/module_interface.hpp>
#include <nodec/event.hpp>

namespace nodec_modules
{
namespace input
{
namespace interfaces
{

class Keyboard : public nodec::ModuleInterface
{
public:
    using nodec::ModuleInterface::ModuleInterface;

public:
    struct Event
    {
        enum class Type
        {
            Press,
            Release
        };

        Event(Type type, Key key) :
            type(type),
            key(key)
        {
        }

        Type type;
        Key key;
    };

public:
    virtual bool get_key_pressed(Key key) const noexcept = 0;
    virtual bool get_key_down(Key key) const noexcept = 0;
    virtual bool get_key_up(Key key) const noexcept = 0;

public:
    nodec::event::Event<unsigned char> on_text_input;
    nodec::event::Event<const Event&> on_keyboard_event;

};
}
}
}

#endif