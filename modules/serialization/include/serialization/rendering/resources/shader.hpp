#ifndef SERIALIZATION__RENDERING__RESOURCES__SHADER_HPP_
#define SERIALIZATION__RENDERING__RESOURCES__SHADER_HPP_


#include <serialization/nodec/vector4.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#include <nodec/vector4.hpp>

#include <vector>
#include <string>


namespace rendering {
namespace resources {


struct SerializableShaderMetaInfo {
    struct FloatProperty {
        std::string name;
        float default_value;

        template<class Archive>
        void serialize(Archive& archive) {
            archive(
                cereal::make_nvp("name", name),
                cereal::make_nvp("default_value", default_value)
            );
        }
    };

    struct Vector4Property {
        std::string name;
        nodec::Vector4f default_value;

        template<class Archive>
        void serialize(Archive& archive) {
            archive(
                cereal::make_nvp("name", name),
                cereal::make_nvp("default_value", default_value)
            );
        }
    };

    struct TextureEntry {
        std::string name;

        template<class Archive>
        void serialize(Archive& archive) {
            archive(
                cereal::make_nvp("name", name)
            );
        }
    };

    std::vector<FloatProperty> float_properties;
    std::vector<Vector4Property> vector4_properties;
    std::vector<TextureEntry> texture_entries;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(
            cereal::make_nvp("float_properties", float_properties),
            cereal::make_nvp("vector4_properties", vector4_properties),
            cereal::make_nvp("texture_entries", texture_entries)
        );
    }
};


}
}

#endif
