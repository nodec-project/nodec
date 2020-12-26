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
    bool get_texture(const std::string& name, TextureEntry& out) const;
    bool set_texture(const std::string& name, const TextureEntry& texture_entry);

protected:
    nodec::NodecObject::Holder<Shader> shader_;

    std::map<std::string, float> float_properties_;
    std::map<std::string, nodec::Vector4f> vector4_properties_;
    std::map<std::string, TextureEntry> texture_properties_;

private:
    template<typename T>
    static bool get_value_generic(const std::map<std::string, T>& properties,
                                  const std::string& name,
                                  T& out);

    template<typename T>
    static bool set_value_generic(std::map<std::string, T>& properties,
                                  const std::string& name,
                                  const T& value);


};
}
}
}

#endif