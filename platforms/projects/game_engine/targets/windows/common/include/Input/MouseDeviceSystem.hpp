#pragma once

#include <nodec_input/input_devices.hpp>
#include <nodec_input/mouse/impl/mouse_device.hpp>

class MouseDeviceSystem {
    using MouseDevice = nodec_input::mouse::impl::MouseDevice;
    using Mouse = nodec_input::mouse::Mouse;
    using RegistryOperations = nodec_input::InputDevices::RegistryOperations<Mouse>;
    using DeviceHandle = nodec_input::InputDevices::DeviceHandle<Mouse>;

public:
    using device_type = Mouse;

    MouseDeviceSystem(RegistryOperations registryOps)
        : mRegistryOps{registryOps} {
        mouse_device_.second = std::make_shared<MouseDevice>();
        mouse_device_.first = registryOps.add_device(mouse_device_.second);
    }

    MouseDevice &device() {
        return *mouse_device_.second;
    }

private:
    std::pair<DeviceHandle, std::shared_ptr<MouseDevice>> mouse_device_;
    RegistryOperations mRegistryOps;

};
