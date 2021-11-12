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

private:
    ShaderPtr shader_;
};

}
}

#endif