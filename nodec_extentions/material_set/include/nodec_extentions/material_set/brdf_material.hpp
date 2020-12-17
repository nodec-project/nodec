#ifndef NODEC_EXTENTIONS__MATERIAL_SET__BRDF_MATERIAL_HPP_
#define NODEC_EXTENTIONS__MATERIAL_SET__BRDF_MATERIAL_HPP_

#include <nodec_modules/rendering/interfaces/material.hpp>

#include <nodec/vector3.hpp>

namespace nodec_extentions
{
namespace material_set
{


class BRDFMaterial : public nodec_modules::rendering::interfaces::Material
{
public:
    struct Properties
    {
        nodec::Vector3f albedo = nodec::Vector3f::ones;
        float roughness = 0.5f;
        float metalness = 0.2f;
    } properties;

    BRDFMaterial();
};


}
}

#endif