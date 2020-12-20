#ifndef NODEC_EXTENTIONS__MATERIAL_SET__BRDF_MATERIAL_HPP_
#define NODEC_EXTENTIONS__MATERIAL_SET__BRDF_MATERIAL_HPP_

#include <nodec_modules/rendering/interfaces/material.hpp>

#include <nodec/vector3.hpp>

namespace nodec_extentions
{
namespace material_set
{


/**
* Properties:
*   metalness float    
*   roughness float    
*   albedo    Vector3f 
* 
*/
class BRDFMaterial final: public nodec_modules::rendering::interfaces::Material
{
private:
    static nodec::NodecObject::Reference<nodec_modules::rendering::interfaces::Shader> brdf_shader_global;

public:
    BRDFMaterial(nodec_modules::rendering::interfaces::Rendering* target_rendering);
};


}
}

#endif