#ifndef NODEC_INPUT__IMPL__INPUT_MODULE_HPP_
#define NODEC_INPUT__IMPL__INPUT_MODULE_HPP_

#include "../input.hpp"
#include "../keyboard/impl/keyboard_impl.hpp"
#include "../mouse/impl/mouse_impl.hpp"

namespace nodec_input {
namespace impl {

class InputModule : public Input {
public:
    virtual keyboard::Keyboard &keyboard() override {
        return keyboard_impl_;
    }

    virtual mouse::Mouse& mouse() override {
        return mouse_impl_;
    }

public:
    keyboard::impl::KeyboardImpl &keyboard_impl() {
        return keyboard_impl_;
    }

    mouse::impl::MouseImpl& mouse_impl() {
        return mouse_impl_;
    }

private:
    keyboard::impl::KeyboardImpl keyboard_impl_;
    mouse::impl::MouseImpl mouse_impl_;
};

} // namespace impl
} // namespace nodec_input

#endif