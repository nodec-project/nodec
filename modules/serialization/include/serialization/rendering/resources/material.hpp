#ifndef SERIALIZATION__RENDERING__RESOURCES__MATERIAL_HPP_
#define SERIALIZATION__RENDERING__RESOURCES__MATERIAL_HPP_

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

#include <serialization/nodec/vector2.hpp>
#include <serialization/nodec/vector3.hpp>
#include <serialization/nodec/vector4.hpp>

#include <rendering/resources/material.hpp>


namespace rendering {
namespace resources {

struct SerializableMaterial {

    struct TextureEntry {
        std::string texture;
        Sampler sampler;

        template<class Archive>
        void serialize(Archive& archive) {
            archive(
                cereal::make_nvp("texture", texture),
                cereal::make_nvp("sampler", sampler)
            );
        }
    };

    std::string shader;

    std::unordered_map<std::string, float> float_properties;
    std::unordered_map<std::string, nodec::Vector4f> vector4_properties;
    std::unordered_map<std::string, TextureEntry> texture_properties;


    template<class Archive>
    void serialize(Archive& archive) {
        archive(
            cereal::make_nvp("shader", shader),
            cereal::make_nvp("float_properties", float_properties),
            cereal::make_nvp("vector4_properties", vector4_properties),
            cereal::make_nvp("texture_properties", texture_properties)
        );
    }
};


}
}

#endif