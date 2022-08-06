#ifndef NODEC_INPUT__INPUT_HPP_
#define NODEC_INPUT__INPUT_HPP_

#include "keyboard/keyboard.hpp"


namespace nodec_input {

class Input {
public:
    
    virtual keyboard::Keyboard& keyboard() = 0;


};

}


#endif