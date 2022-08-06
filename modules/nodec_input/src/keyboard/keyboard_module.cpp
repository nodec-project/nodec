#include <input/keyboard/impl/keyboard_module.hpp>

namespace input {
namespace keyboard {
namespace impl {

bool KeyboardModule::get_key_pressed(Key key) const noexcept {
    return key_states[static_cast<int>(key)];
}

bool KeyboardModule::get_key_down(Key key) const noexcept {
    auto code = static_cast<int>(key);
    return key_states[code] && (key_states[code] != key_states_prev[code]);
}

bool KeyboardModule::get_key_up(Key key) const noexcept {
    auto code = static_cast<int>(key);
    return !key_states[code] && (key_states[code] != key_states_prev[code]);
}

void KeyboardModule::flush() {
    key_states_prev = key_states;

    while (!event_queue.empty()) {
        Event& key_event = event_queue.front();
        switch (key_event.type) {
        case Event::Type::Press:
            key_states[static_cast<int>(key_event.key)] = true;
            break;

        case Event::Type::Release:
            key_states[static_cast<int>(key_event.key)] = false;
            break;
        }
        on_keyboard_event_(key_event);
        event_queue.pop();
    }

    while (!text_queue.empty()) {
        auto text = text_queue.front();
        on_text_input_(text);
        text_queue.pop();
    }
}

void KeyboardModule::handle_key_press(Key key) {
    event_queue.push({ Event::Type::Press, key });
}

void KeyboardModule::handle_key_release(Key key) {
    event_queue.push({ Event::Type::Release, key });
}

void KeyboardModule::handle_text_input(unsigned char text) {
    text_queue.push(text);
}

}
}
} // namespace input