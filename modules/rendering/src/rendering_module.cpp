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

        void RenderingModule::bind_mesh(interfaces::Mesh* mesh)
        {
            on_bind_mesh.invoke(mesh);
        }

        void RenderingModule::unbind_mesh(interfaces::Mesh* mesh)
        {
            on_unbind_mesh.invoke(mesh);
        }

        void RenderingModule::bind_shader(interfaces::Shader* shader)
        {
            on_bind_shader.invoke(shader);
        }
        void RenderingModule::unbind_shader(interfaces::Shader* shader)
        {
            on_unbind_shader.invoke(shader);
        }

        void RenderingModule::bind_material(interfaces::Material* material)
        {
            on_bind_material.invoke(material);
        }
        void RenderingModule::unbind_material(interfaces::Material* material)
        {
            on_unbind_material.invoke(material);
        }

        void RenderingModule::regist_renderer(nodec::NodecObject::Reference<interfaces::Renderer> renderer)
        {
            on_regist_renderer.invoke(renderer);
        }
    }
}