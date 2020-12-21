#include <nodec_extentions/material_set/brdf_material.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

using namespace nodec_modules::rendering::interfaces;

namespace nodec_extentions
{
namespace material_set
{

nodec::NodecObject::Reference<Shader> BRDFMaterial::brdf_shader_global;

BRDFMaterial::BRDFMaterial(Rendering* target_rendering) :
    Material(target_rendering, NodecObject::Holder<Shader>())
{
    if (auto brdf_shader = brdf_shader_global.lock())
    {
        shader_ = brdf_shader;
    }
    else
    {
        auto new_shader = NodecObject::instanciate<Shader>("brdf", target_rendering);
        brdf_shader_global = new_shader;
        shader_ = new_shader;
        target_rendering->bind_shader(shader_.get());
    }

    float_properties_.emplace("metalness", 0.2f);
    float_properties_.emplace("roughness", 0.5f);
    vector4_properties_.emplace("albedo", nodec::Vector4f::ones);
}

} // namespace material_set
} // namespace nodec_extentions