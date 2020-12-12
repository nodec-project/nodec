#include <nodec_modules/rendering/interfaces/material.hpp>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

Material::Material(nodec::NodecObject::Holder<Shader> shader, Rendering* target_rendering) :
    BindableResource(target_rendering, "Material"),
    shader(shader)
{
}

}
}
}