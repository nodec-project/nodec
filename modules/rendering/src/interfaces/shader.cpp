#include <nodec_modules/rendering/interfaces/shader.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

Shader::Shader(const std::string& shader_name, Rendering* target_rendering) :
    BindableResource(target_rendering, "Shader::" + shader_name),
    shader_name(shader_name)
{

}

Shader::~Shader()
{
    target_rendering->unbind_shader(this);
}

}
}
}