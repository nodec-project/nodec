#ifndef NODEC_MODULES__INPUT__INTERFACES__MOUSE_HPP_
#define NODEC_MODULES__INPUT__INTERFACES__MOUSE_HPP_

#include <nodec/module_interface.hpp>

namespace nodec_modules
{
namespace input
{
namespace interfaces
{

class Mouse : public nodec::ModuleInterface
{
public:
    using ModuleInterface::ModuleInterface;

};

} // namespace interfaces
} // namespace input
} // namespace nodec_modules

#endif