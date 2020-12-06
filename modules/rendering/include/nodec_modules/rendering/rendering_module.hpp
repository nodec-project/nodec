#ifndef NODEC_MODULES__RENDERING__RENDERING_MODULE_HPP_
#define NODEC_MODULES__RENDERING__RENDERING_MODULE_HPP_

#include "interfaces/rendering.hpp"

namespace nodec_modules
{
    namespace rendering
    {
        class RenderingModule : public interfaces::Rendering
        {
        public:
            RenderingModule();

        public:
            float frame_delta_time() const noexcept override;

        public:
            float frame_delta_time_;
        };
    }
}

#endif