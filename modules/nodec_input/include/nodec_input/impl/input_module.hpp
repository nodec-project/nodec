#ifndef NODEC_INPUT__IMPL__INPUT_MODULE_HPP_
#define NODEC_INPUT__IMPL__INPUT_MODULE_HPP_

#include "../input.hpp"
#include "../keyboard/impl/keyboard_impl.hpp"


namespace nodec_input {
namespace impl {

class InputModule : public Input {
public:

    virtual keyboard::Keyboard& keyboard() override {
        return keyboard_impl_;
    }

    keyboard::impl::KeyboardImpl& keyboard_impl() {
        return keyboard_impl_;
    }


private:
    keyboard::impl::KeyboardImpl keyboard_impl_;

};

}
}


#endif