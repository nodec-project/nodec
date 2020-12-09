#include <nodec_modules/rendering/rendering_module.hpp>

namespace nodec_modules
{
    namespace rendering
    {
        RenderingModule::RenderingModule():
            interfaces::Rendering("nodec_modules::rendering::RenderingModule"),
            frame_delta_time_(0)
        {

        }

        float RenderingModule::frame_delta_time() const noexcept
        {
            return frame_delta_time_;
        }

        void RenderingModule::publish_mesh(interfaces::Mesh* mesh)
        {
            on_publish_mesh.invoke(mesh);
        }

        void RenderingModule::discard_mesh(interfaces::Mesh* mesh)
        {
            on_discard_mesh.invoke(mesh);
        }

        void RenderingModule::publish_shader(interfaces::Shader* shader)
        {
            on_publish_shader.invoke(shader);
        }
        void RenderingModule::discard_shader(interfaces::Shader* shader)
        {
            on_discard_shader.invoke(shader);
        }

        void RenderingModule::regist_renderer(nodec::NodecObject::Reference<interfaces::Renderer> renderer)
        {
            on_regist_renderer.invoke(renderer);
        }
    }
}