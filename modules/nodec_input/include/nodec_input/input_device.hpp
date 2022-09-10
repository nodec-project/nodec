#ifndef NODEC_INPUT__INPUT_DEVICE_HPP_
#define NODEC_INPUT__INPUT_DEVICE_HPP_

namespace nodec_input {

class InputDevice {
public:
    enum class State {
        Disconnected,
        Connected
    };
};
} // namespace nodec_input

#endif