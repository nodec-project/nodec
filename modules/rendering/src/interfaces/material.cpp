#include <nodec_modules/rendering/interfaces/material.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

Material::Material(Rendering* target_rendering,
                   nodec::NodecObject::Holder<Shader> shader,
                   void* primitive_properties_entry_ptr,
                   size_t primitive_properties_byte_size) :
    BindableResource(target_rendering, "Material"),
    shader_(shader),
    primitive_properties_entry_ptr_(primitive_properties_entry_ptr),
    primitive_properties_byte_size_(primitive_properties_byte_size)
{
}

Material::~Material()
{
    target_rendering->unbind_material(this);
}

const Shader& Material::shader() const noexcept
{
    return *shader_;
}

void* Material::primitive_properties_entry_ptr() const noexcept
{
    return primitive_properties_entry_ptr_;
}
size_t Material::primitive_properties_byte_size() const noexcept
{
    return primitive_properties_byte_size_;
}

}
}
}