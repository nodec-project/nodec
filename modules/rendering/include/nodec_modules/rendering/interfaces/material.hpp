#ifndef NODEC_MODULES__RENDERING__INTERFACES__MATERIAL_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__MATERIAL_HPP_

#include "shader.hpp"

#include <nodec/nodec_object.hpp>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            class Material : public nodec::NodecObject
            {
            public:
                Material(const std::string& shader_name);

            private:
                nodec::NodecObject::Holder<Shader> shader;
            };
        }
    }
}

#endif