#include <nodec_extentions/material_set/brdf_material.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

using namespace nodec_modules::rendering::interfaces;

namespace nodec_extentions
{
namespace material_set
{

BRDFMaterial::BRDFMaterial(Rendering* target_rendering) :
    Material(target_rendering, NodecObject::Holder<Shader>(), &properties, sizeof(Properties))
{
    if (auto brdf_shader = brdf_shader_global.lock())
    {
        shader = brdf_shader;
    }
    else
    {
        auto new_shader = NodecObject::instanciate<Shader>("brdf", target_rendering);
        brdf_shader_global = new_shader;
        shader = new_shader;
        target_rendering->bind_shader(shader.get());
    }
}

}
}