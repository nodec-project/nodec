#include <nodec_modules/rendering/interfaces/shader.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            Shader::Shader(const std::string& shader_name) :
                NodecObject("Shader"),
                shader_name(shader_name)
            {

            }

            Shader::~Shader()
            {
                unbind();
            }

            void Shader::bind_impl(Rendering* rendering)
            {
                rendering->bind_shader(this);
            }

            void Shader::unbind_impl(Rendering* rendering)
            {
                rendering->unbind_shader(this);
            }
        }
    }
}