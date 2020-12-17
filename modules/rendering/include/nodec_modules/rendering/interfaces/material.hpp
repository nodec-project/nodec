#ifndef NODEC_MODULES__RENDERING__INTERFACES__MATERIAL_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__MATERIAL_HPP_

#include "shader.hpp"

#include <nodec/nodec_object.hpp>

#include <vector>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

class Material : public BindableResource
{
public:
    Material(Rendering* target_rendering, 
             nodec::NodecObject::Holder<Shader> shader,
             void* primitive_properties_entry_ptr,
             size_t primitive_properties_byte_size);

    void* primitive_properties_entry_ptr() const noexcept;
    size_t primitive_properties_byte_size() const noexcept;

protected:
    nodec::NodecObject::Holder<Shader> shader;

private:
    void* primitive_properties_entry_ptr_;
    size_t primitive_properties_byte_size_;


};
}
}
}

#endif