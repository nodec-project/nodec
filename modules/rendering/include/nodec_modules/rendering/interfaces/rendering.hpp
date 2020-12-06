#ifndef NODEC_MODULES__RENDERING__INTERFACES__RENDERING_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__RENDERING_HPP_

#include <nodec/module_interface.hpp>
#include <nodec/event.hpp>


namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            class Rendering : public nodec::ModuleInterface
            {
            public:
                using ModuleInterface::ModuleInterface;

            public:
                virtual float frame_delta_time() const noexcept = 0;

                nodec::event::Event<Rendering&> on_frame_update;
            };
        }
    }
}
#endif