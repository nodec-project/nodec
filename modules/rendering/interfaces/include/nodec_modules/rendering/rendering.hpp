#ifndef NODEC_MODULES__RENDERING__RENDERING_HPP_
#define NODEC_MODULES__RENDERING__RENDERING_HPP_

#include <nodec/module_interface.hpp>

namespace nodec_modules
{
    namespace rendering
    {
        class Rendering : public nodec::ModuleInterface
        {
        public:
            using ModuleInterface::ModuleInterface;
        };
    }
}
#endif