#ifndef SERIALIZATION__NODEC_RENDERING__RESOURCES__SHADER_HPP_
#define SERIALIZATION__NODEC_RENDERING__RESOURCES__SHADER_HPP_

#include <serialization/nodec/vector4.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#include <nodec/vector4.hpp>

#include <string>
#include <vector>

namespace nodec_rendering {
namespace resources {

struct SerializableShaderMetaInfo {
    struct FloatProperty {
        std::string name;
        float default_value;
    };

    struct Vector4Property {
        std::string name;
        nodec::Vector4f default_value;
    };

    struct TextureEntry {
        std::string name;
    };

    std::vector<FloatProperty> float_properties;
    std::vector<Vector4Property> vector4_properties;
    std::vector<TextureEntry> texture_entries;
};

template<class Archive>
void serialize(Archive &archive, SerializableShaderMetaInfo::FloatProperty &property) {
    archive(
        cereal::make_nvp("name", property.name),
        cereal::make_nvp("default_value", property.default_value));
}

template<class Archive>
void serialize(Archive &archive, SerializableShaderMetaInfo::Vector4Property &property) {
    archive(
        cereal::make_nvp("name", property.name),
        cereal::make_nvp("default_value", property.default_value));
}

template<class Archive>
void serialize(Archive &archive, SerializableShaderMetaInfo::TextureEntry &entry) {
    archive(
        cereal::make_nvp("name", entry.name));
}

template<class Archive>
void serialize(Archive &archive, SerializableShaderMetaInfo &info) {
    archive(
        cereal::make_nvp("float_properties", info.float_properties),
        cereal::make_nvp("vector4_properties", info.vector4_properties),
        cereal::make_nvp("texture_entries", info.texture_entries));
}

} // namespace resources
} // namespace nodec_rendering

#endif
