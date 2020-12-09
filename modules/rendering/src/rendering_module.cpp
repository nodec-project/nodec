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

        void RenderingModule::publish_mesh(const interfaces::Mesh& mesh)
        {
            on_publish_mesh.invoke(mesh);
        }
    }
}