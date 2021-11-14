#ifndef RENDERING__RESOURCES__MATERIAL_HPP_
#define RENDERING__RESOURCES__MATERIAL_HPP_

#include "shader.hpp"
#include "texture.hpp"
#include "sampler.hpp"

#include <nodec/vector4.hpp>

#include <map>
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
    virtual float get_float_property(const std::string& name) = 0;
    virtual void set_float_property(const std::string& name, const float& value) = 0;

    virtual nodec::Vector4f get_vector4_property(const std::string& name) = 0;
    virtual void set_vector4_property(const std::string& name, const nodec::Vector4f& value) = 0;

    virtual TextureEntry get_texture_entry(const std::string& name) = 0;
    virtual void set_texture_entry(const std::string& name, const TextureEntry& value) = 0;


private:
    ShaderPtr shader_;
};

}
}

#endif