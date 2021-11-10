#ifndef SERIALIZATION__RENDERING__RESOURCES__MATERIAL_HPP_
#define SERIALIZATION__RENDERING__RESOURCES__MATERIAL_HPP_

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

#include <serialization/nodec/vector4.hpp>
#include <serialization/nodec/vector3.hpp>
#include <serialization/nodec/vector2.hpp>

#include <rendering/resources/material.hpp>


namespace rendering {
namespace resources {


template<class Archive>
void serialize(Archive& archive, Material::TextureEntry& entry) {
    std::string texture_name = entry.texture ? entry.texture->name() : "";

    archive(
        cereal::make_nvp("texture", texture_name),
        cereal::make_nvp("sampler", entry.sampler)
    );
}


template<class Archive>
void serialize(Archive& archive, Material& material) {
    std::string shader_name = material.shader() ? material.shader()->name() : "";

    archive(
        cereal::make_nvp("shader", shader_name),
        cereal::make_nvp("float_properties", material.float_properties),
        cereal::make_nvp("vector4_properties", material.vector4_properties),
        cereal::make_nvp("texture_properties", material.texture_properties)
    );
}


}
}

#endif