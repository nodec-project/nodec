#pragma once

#include <nodec_input/input_devices.hpp>
#include <nodec_input/keyboard/impl/keyboard_device.hpp>

class KeyboardDeviceSystem {
    using KeyboardDevice = nodec_input::keyboard::impl::KeyboardDevice;
    using Keyboard = nodec_input::keyboard::Keyboard;
    using RegistryOperations = nodec_input::InputDevices::RegistryOperations<Keyboard>;
    using DeviceHandle = nodec_input::InputDevices::DeviceHandle<Keyboard>;

public:
    using device_type = Keyboard;
    
    KeyboardDeviceSystem(RegistryOperations registryOps)
        : mRegistryOps{registryOps} {
        keyboard_device_.second = std::make_shared<KeyboardDevice>();
        keyboard_device_.first = registryOps.add_device(keyboard_device_.second);
    }

    KeyboardDevice &device() {
        return *keyboard_device_.second;
    }

private:
    std::pair<DeviceHandle, std::shared_ptr<KeyboardDevice>> keyboard_device_;
    RegistryOperations mRegistryOps;
};
