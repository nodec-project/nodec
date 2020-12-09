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

            void publish_mesh(interfaces::Mesh* mesh) override;
            void discard_mesh(interfaces::Mesh* mesh) override;

            void publish_shader(interfaces::Shader* shader) override;
            void discard_shader(interfaces::Shader* shader) override;

            void regist_renderer(nodec::NodecObject::Reference<interfaces::Renderer> renderer) override;

        public:
            float frame_delta_time_;

            nodec::event::Event<interfaces::Mesh*> on_publish_mesh;
            nodec::event::Event<interfaces::Mesh*> on_discard_mesh;
            nodec::event::Event<interfaces::Shader*> on_publish_shader;
            nodec::event::Event<interfaces::Shader*> on_discard_shader;

            nodec::event::Event<nodec::NodecObject::Reference<interfaces::Renderer>> on_regist_renderer;

        };
    }
}

#endif