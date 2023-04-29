#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__RESOURCES__MATERIAL_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__RESOURCES__MATERIAL_HPP_

#include "../sampler.hpp"

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>
#include <nodec/resource_management/resource_registry.hpp>

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

    SerializableMaterial() = default;

    SerializableMaterial(const Material &source, nodec::resource_management::ResourceRegistry &registry) {
        auto source_shader = source.shader();
        
        shader = registry.lookup_name<Shader>(source_shader).first;
        cull_mode = source.cull_mode();

        if (source_shader) {
            for (std::size_t i = 0; i < source_shader->float_property_count(); ++i) {
                auto name = source_shader->get_float_property_name(i);
                float_properties[name] = source.get_float_property(name);
            }
            for (std::size_t i = 0; i < source_shader->vector4_property_count(); ++i) {
                auto name = source_shader->get_vector4_property_name(i);
                vector4_properties[name] = source.get_vector4_property(name).value();
            }
            for (std::size_t i = 0; i < source_shader->texture_entry_count(); ++i) {
                const auto name = source_shader->get_texture_entry_name(i);
                auto entry = source.get_texture_entry(name).value();
                if (!entry.texture) continue;

                auto &ser_entry = texture_properties[name];
                ser_entry.texture = registry.lookup_name<Texture>(entry.texture).first;
                ser_entry.sampler = entry.sampler;
            }
        }
    }

    void apply_to(Material &dest, nodec::resource_management::ResourceRegistry &registry) {
        auto shader_ptr = registry.get_resource_direct<Shader>(shader);
        dest.set_shader(shader_ptr);

        for (auto &&property : float_properties) {
            dest.set_float_property(property.first, property.second);
        }

        for (auto &&property : vector4_properties) {
            dest.set_vector4_property(property.first, property.second);
        }

        for (auto &&property : texture_properties) {
            auto &ser_entry = property.second;

            Material::TextureEntry entry;
            entry.sampler = ser_entry.sampler;

            auto texture = registry.get_resource_direct<Texture>(ser_entry.texture);
            entry.texture = texture;

            dest.set_texture_entry(property.first, entry);
        }

        dest.set_cull_mode(cull_mode);
    }

    // apply_from?
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