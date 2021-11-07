#include <nodec_modules/rendering/interfaces/material.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{


const std::map<std::string, float>&
Material::float_properties() const
{
    return float_properties_;
}
bool Material::get_float(const std::string& name, float& out) const
{
    auto iter = float_properties_.find(name);
    if (iter == float_properties_.end())
    {
        return false;
    }
    out = iter->second;
    return true;
}
bool Material::set_float(const std::string& name, const float& value)
{
    auto iter = float_properties_.find(name);
    if (iter == float_properties_.end())
    {
        return false;
    }
    iter->second = value;
    return true;
}

const std::map<std::string, nodec::Vector4f>&
Material::vector4_properties() const
{
    return vector4_properties_;
}
bool Material::get_vector4(const std::string& name, nodec::Vector4f& out) const
{
    auto iter = vector4_properties_.find(name);
    if (iter == vector4_properties_.end())
    {
        return false;
    }
    out = iter->second;
    return true;
}
bool Material::set_vector4(const std::string& name, const nodec::Vector4f& value)
{
    auto iter = vector4_properties_.find(name);
    if (iter == vector4_properties_.end())
    {
        return false;
    }
    iter->second = value;
    return true;
}


const std::map<std::string, Material::TextureEntry>&
Material::texture_properties() const
{
    return texture_properties_;
}

bool Material::get_texture(const std::string& name, NodecObject::Holder<Texture>& out) const
{
    auto iter = texture_properties_.find(name);
    if (iter == texture_properties_.end())
    {
        return false;
    }
    out = iter->second.texture;
    return true;
}

bool Material::set_texture(const std::string& name, const NodecObject::Holder<Texture>& texture)
{
    auto iter = texture_properties_.find(name);
    if (iter == texture_properties_.end())
    {
        return false;
    }
    iter->second.texture = texture;
    return true;
}

bool Material::get_texture_sampler(const std::string& name, Sampler& out) const
{
    auto iter = texture_properties_.find(name);
    if (iter == texture_properties_.end())
    {
        return false;
    }
    out = iter->second.sampler;
    return true;
}

bool Material::set_texture_sampler(const std::string& name, const Sampler& sampler)
{
    auto iter = texture_properties_.find(name);
    if (iter == texture_properties_.end())
    {
        return false;
    }
    iter->second.sampler = sampler;
    return true;
}



} // namespace interfaces
} // namespace rendering
} // namespace nodec_modules