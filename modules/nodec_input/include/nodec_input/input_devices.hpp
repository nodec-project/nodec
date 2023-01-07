#ifndef NODEC_INPUT__INPUT_HPP_
#define NODEC_INPUT__INPUT_HPP_

#include <nodec/memory.hpp>
#include <nodec/signals.hpp>
#include <nodec/type_info.hpp>

#include <list>
#include <memory>
#include <queue>
#include <vector>

namespace nodec_input {

class InputDevices {
    template<typename Device>
    struct DeviceBlock {
        DeviceBlock() {}
        std::list<std::shared_ptr<Device>> devices;
        std::shared_ptr<void> system;
    };

    template<typename Device>
    decltype(auto) device_block_assured() {
        const auto index = nodec::type_seq_index<Device>::value();
        if (!(index < device_blocks_.size())) device_blocks_.resize(index + 1);
        auto &block = device_blocks_[index];
        if (!block) {
            block = std::make_shared<DeviceBlock<Device>>();
        }
        return *static_cast<DeviceBlock<Device> *>(block.get());
    }

    template<typename Device>
    const DeviceBlock<Device> &device_block_assured() const {
        const auto index = nodec::type_seq_index<Device>::value();
        const DeviceBlock<Device> *block{nullptr};

        if (index < device_blocks_.size()) {
            block = static_cast<const DeviceBlock<Device> *>(device_blocks_[index].get());
        }

        if (block != nullptr) {
            return *block;
        }

        static DeviceBlock<Device> placeholder{};
        return placeholder;
    }

public:
    template<typename Device>
    using DeviceHandle = typename std::list<std::shared_ptr<Device>>::iterator;

    template<typename Device>
    class RegistryOperations {
    public:
        RegistryOperations(InputDevices *devices)
            : devices_{devices} {}

        decltype(auto) add_device(std::shared_ptr<Device> device) {
            return devices_->add_device(device);
        }

        decltype(auto) remove_device(DeviceHandle<Device> device_handle) {
            return devices_->remove_device(device_handle);
        }

        InputDevices &devices() {
            return *devices_;
        }

        const InputDevices &devices() const {
            return *devices_;
        }

    private:
        InputDevices *devices_;
    };

    template<typename System, class... Args>
    System &emplace_device_system(Args &&...args) {
        using Device = typename System::device_type;
        DeviceBlock<Device> &block = device_block_assured<Device>();
        block.system = std::make_shared<System>(std::forward<Args>(args)..., RegistryOperations<Device>{this});
        return *static_cast<System *>(block.system.get());
    }

    template<typename Device>
    const std::list<std::shared_ptr<Device>> &get_available_devices() const {
        const auto &block = device_block_assured<Device>();
        return block.devices;
    }

private:
    template<typename Device>
    DeviceHandle<Device> add_device(std::shared_ptr<Device> device) {
        assert(static_cast<bool>(device));

        auto &block = device_block_assured<Device>();
        auto iter = block.devices.insert(block.devices.end(), device);
        // TODO: device_added_()
        return iter;
    }

    template<typename Device>
    void remove_device(DeviceHandle<Device> device_handle) {
        auto &block = device_block_assured<Device>();
        block.devices.erace(device_handle);
        // TODO: device_removed_()
    }

private:
    // nodec::signals::Signal<void(std::shared_ptr<InputDevice>)> device_added_;
    // nodec::signals::Signal<void(std::shared_ptr<InputDevice>)> device_removed_;

    std::vector<std::shared_ptr<void>> device_blocks_;
};
} // namespace nodec_input

#endif