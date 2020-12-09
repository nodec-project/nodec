#ifndef NODEC_MODULES__RENDERING__INTERFACES__RENDERING_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__RENDERING_HPP_

#include "mesh.hpp"
#include "shader.hpp"
#include "renderer.hpp"

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
                
                virtual void publish_mesh(Mesh* mesh) = 0;
                virtual void discard_mesh(Mesh* mesh) = 0;

                virtual void publish_shader(Shader* shader) = 0;
                virtual void discard_shader(Shader* shader) = 0;

                virtual void regist_renderer(nodec::NodecObject::Reference<Renderer> renderer) = 0;

            public:
                nodec::event::Event<Rendering&> on_frame_update;
            };
        }
    }
}
#endif