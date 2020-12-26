#ifndef NODEC_MODULES__RENDERING__INTERFACES__MATERIAL_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__MATERIAL_HPP_

#include "shader.hpp"
#include "sampler.hpp"
#include "texture.hpp"

#include <nodec/nodec_object.hpp>
#include <nodec/vector4.hpp>

#include <map>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

class Material : public BindableResource
{
public:
    Material(nodec::NodecObject::Holder<Shader> shader,
             Rendering* target_rendering);

    ~Material();

public:
    struct TextureEntry
    {
        TextureEntry(NodecObject::Holder<Texture> texture, Sampler sampler) :
            texture(texture), sampler(sampler)
        {
        }

        TextureEntry() :
            sampler(Sampler::Bilinear)
        {
        }

        NodecObject::Holder<Texture> texture;
        Sampler sampler;
    };
public:
    const Shader& shader() const noexcept;

    const std::map<std::string, float>& float_properties() const;
    bool get_float(const std::string& name, float& out) const;
    bool set_float(const std::string& name, const float& value);

    const std::map<std::string, nodec::Vector4f>& vector4_properties() const;
    bool get_vector4(const std::string& name, nodec::Vector4f& out) const;
    bool set_vector4(const std::string& name, const nodec::Vector4f& value);

    const std::map<std::string, TextureEntry>& texture_properties() const;
    bool get_texture(const std::string& name, NodecObject::Holder<Texture>& out) const;
    bool set_texture(const std::string& name, const NodecObject::Holder<Texture>& texture);

    bool get_texture_sampler(const std::string& name, Sampler& out) const;
    bool set_texture_sampler(const std::string& name, const Sampler& sampler);

protected:
    nodec::NodecObject::Holder<Shader> shader_;

    std::map<std::string, float> float_properties_;
    std::map<std::string, nodec::Vector4f> vector4_properties_;
    std::map<std::string, TextureEntry> texture_properties_;



};

} // namespace interfaces
} // namespace rendering
} // namespace nodec_modules

#endif