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
class Material : public BindableResource
{
public:
    Material(nodec::NodecObject::Holder<Shader> shader, Rendering* target_rendering);

private:
    nodec::NodecObject::Holder<Shader> shader;
};
}
}
}

#endif