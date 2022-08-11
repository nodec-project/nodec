#ifndef NODEC_RENDERING__RESOURCES__MATERIAL_HPP_
#define NODEC_RENDERING__RESOURCES__MATERIAL_HPP_

#include "sampler.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <nodec/vector4.hpp>

#include <map>
#include <memory>

namespace nodec_rendering {
namespace resources {

class Material {
public:
    using ShaderPtr = std::shared_ptr<Shader>;
    using TexturePtr = std::shared_ptr<Texture>;

public:
    void set_shader(ShaderPtr shader) {
        if (shader_ == shader) {
            return;
        }

        shader_ = shader;
        on_shader_changed();
    }

    ShaderPtr shader() const {
        return shader_;
    }

public:
    struct TextureEntry {
        TextureEntry(TexturePtr texture, Sampler sampler)
            : texture(texture), sampler(sampler) {
        }

        TextureEntry()
            : sampler(Sampler::Bilinear) {
        }

        TexturePtr texture;
        Sampler sampler;
    };

public:
    virtual float get_float_property(const std::string &name) const = 0;
    virtual void set_float_property(const std::string &name, const float &value) = 0;

    virtual nodec::Vector4f get_vector4_property(const std::string &name) const = 0;
    virtual void set_vector4_property(const std::string &name, const nodec::Vector4f &value) = 0;

    virtual TextureEntry get_texture_entry(const std::string &name) const = 0;
    virtual void set_texture_entry(const std::string &name, const TextureEntry &value) = 0;

protected:
    virtual void on_shader_changed() = 0;

private:
    ShaderPtr shader_;
};

} // namespace resources
} // namespace nodec_rendering

#endif