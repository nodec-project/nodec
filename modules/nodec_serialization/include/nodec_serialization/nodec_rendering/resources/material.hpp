#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__RESOURCES__MATERIAL_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__RESOURCES__MATERIAL_HPP_

#include "../sampler.hpp"

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

#include <nodec_serialization/nodec/vector2.hpp>
#include <nodec_serialization/nodec/vector3.hpp>
#include <nodec_serialization/nodec/vector4.hpp>

#include <nodec_rendering/resources/material.hpp>

namespace nodec_rendering {
namespace resources {

struct SerializableMaterial {
    struct TextureEntry {
        std::string texture;
        Sampler sampler;
    };

    std::string shader;

    std::unordered_map<std::string, float> float_properties;
    std::unordered_map<std::string, nodec::Vector4f> vector4_properties;
    std::unordered_map<std::string, TextureEntry> texture_properties;

    CullMode cull_mode{CullMode::Back};
};
template<class Archive>
void serialize(Archive &archive, SerializableMaterial::TextureEntry &entry) {
    archive(
        cereal::make_nvp("texture", entry.texture),
        cereal::make_nvp("sampler", entry.sampler));
}

template<class Archive>
void serialize(Archive &archive, SerializableMaterial &material) {
    archive(
        cereal::make_nvp("shader", material.shader),
        cereal::make_nvp("cull_mode", material.cull_mode),
        cereal::make_nvp("float_properties", material.float_properties),
        cereal::make_nvp("vector4_properties", material.vector4_properties),
        cereal::make_nvp("texture_properties", material.texture_properties));
}

} // namespace resources
} // namespace nodec_rendering

#endif