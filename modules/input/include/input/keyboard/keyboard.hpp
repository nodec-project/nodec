#ifndef INPUT__KEYBOARD__KEYBOARD_HPP_
#define INPUT__KEYBOARD__KEYBOARD_HPP_

#include <input/keyboard/key.hpp>

#include <nodec/event.hpp>

namespace input
{
namespace keyboard
{

class Keyboard
{
private:
    NODEC_DISABLE_COPY(Keyboard);

public:
    Keyboard() {};

public:

    struct Event
    {
        enum class Type
        {
            Press,
            Release
        };

        Event(Type type, Key key):
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

#endif