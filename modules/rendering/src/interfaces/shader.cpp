#include <nodec_modules/rendering/interfaces/shader.hpp>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            Shader::Shader(const char* shader_name) :
                NodecObject("Shader"),
                shader_name(shader_name)
            {

            }

            Shader::~Shader()
            {

            }
        }
    }
}