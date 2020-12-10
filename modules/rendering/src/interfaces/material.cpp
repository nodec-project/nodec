#include <nodec_modules/rendering/interfaces/material.hpp>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            Material::Material(const std::string& shader_name):
                NodecObject("Material")
            {
                shader = nodec::NodecObject::instanciate<Shader>(shader_name);
            }
        }
    }
}