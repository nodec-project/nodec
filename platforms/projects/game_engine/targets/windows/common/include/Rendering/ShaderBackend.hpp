#pragma once

#include <rendering/resources/shader.hpp>

#include <nodec/vector4.hpp>

#include <vector>
#include <string>


class ShaderBackend : public rendering::resources::Shader {

public:

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

    struct MetaInfo {
        std::vector<FloatProperty> float_properties;
        std::vector<Vector4Property> vector4_properties;
        std::vector<TextureEntry> texture_entries;
    };

public:
    ShaderBackend(const MetaInfo& meta_info) {

    }

    std::vector<uint8_t> create_property_memory() {

    }

    float get_float_property(const std::vector<uint8_t>& property_memory, const std::string& name) const {

    }

    void set_float_property(std::vector<uint8_t>& property_memory, const std::string& name, const float& value) const {

    }

    nodec::Vector4f get_vector4_property(const std::vector<uint8_t>& property_memory, const std::string& name) const {

    }

    const auto& float_properties() const {
        return float_properties_;
    }

    const auto& vector4_properties() const {
        return vector4_properties_;
    }

    const auto& texture_entries() const {
        return texture_entries_;
    }


private:
    std::vector<FloatProperty> float_properties_;
    std::unordered_map<std::string, int> float_name_to_index_;

    std::vector<Vector4Property> vector4_properties_;
    std::unordered_map<std::string, int> vector4_name_to_index_;

    std::vector<TextureEntry> texture_entries_;



};