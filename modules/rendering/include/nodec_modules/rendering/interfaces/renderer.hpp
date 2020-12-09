#ifndef NODEC_MODULES__RENDERING__INTERFACES__RENDERER_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__RENDERER_HPP_

#include "mesh.hpp"

#include <nodec/scene_set/component.hpp>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            class Renderer : public nodec::scene_set::Component
            {
            public:
                using Component::Component;

            public:
                nodec::NodecObject::Holder<Mesh> mesh;

            };
        };
    }
}

#endif