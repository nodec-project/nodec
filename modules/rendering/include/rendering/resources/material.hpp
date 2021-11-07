#ifndef RENDERING__RESOURCES__MATERIAL_HPP_
#define RENDERING__RESOURCES__MATERIAL_HPP_

#include "shader.hpp"
#include "texture.hpp"
#include "sampler.hpp"

#include <memory>


namespace rendering {
namespace resources {


class Material {
    using ShaderPtr = std::shared_ptr<Shader>;
    using TexturePtr = std::shared_ptr<Texture>;
    

public:
    void set_shader(ShaderPtr shader) {
        shader_ = shader;
    }

    ShaderPtr shader() const {
        return shader_;
    }

    void reset_shader() {
        shader_.reset();
    }

public:
    struct TextureEntry {
        TextureEntry(TexturePtr texture, Sampler sampler)
            : texture(texture)
            , sampler(sampler) {
        }

        TextureEntry()
            : sampler(Sampler::Bilinear) {
        }

        TexturePtr texture;
        Sampler sampler;
    };

public:
    virtual bool get_float(const std::string& name, float& out) const = 0;
    virtual bool set_float(const std::string& name, const float& value) = 0;

    virtual bool get_vector4(const std::string& name, nodec::Vector4f& out) const = 0;
    virtual bool set_vector4(const std::string& name, const nodec::Vector4f& value) = 0;

    virtual bool get_texture(const std::string& name, TexturePtr& out) const = 0;
    virtual bool set_texture(const std::string& name, const TexturePtr& texture) = 0;

    virtual bool get_texture_sampler(const std::string& name, Sampler& out) const = 0;
    virtual bool set_texture_sampler(const std::string& name, const Sampler& sampler) = 0;

private:
    ShaderPtr shader_;
};

}
}

#endif