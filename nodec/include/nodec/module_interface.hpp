#ifndef NODEC__MODULE_INTERFACE_HPP_
#define NODEC__MODULE_INTERFACE_HPP_

namespace nodec
{
    class ModuleInterface
    {
    public:
        ModuleInterface() = default;
        virtual ~ModuleInterface() {};
        
        ModuleInterface(const ModuleInterface&) = delete;
        ModuleInterface& operator=(const ModuleInterface&) = delete;
    };
}

#endif