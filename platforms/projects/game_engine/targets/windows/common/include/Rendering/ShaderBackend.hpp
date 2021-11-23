#pragma once

#include <Graphics/InputLayout.hpp>
#include <Graphics/VertexShader.hpp>
#include <Graphics/PixelShader.hpp>

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
    ShaderBackend(Graphics *graphics, const std::string& path, const MetaInfo& meta_info) {
        using namespace nodec;

        float_properties_ = meta_info.float_properties;
        vector4_properties_ = meta_info.vector4_properties;
        texture_entries_ = meta_info.texture_entries;

        vertex_shader_.reset(new VertexShader(graphics, Formatter() << path << "/vertex.cso"));
        pixel_shader_.reset(new PixelShader(graphics, Formatter() << path << "/pixel.cso"));

        const D3D11_INPUT_ELEMENT_DESC ied[] = {
            { "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                           , D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL"   , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        input_layout_.reset(new InputLayout(graphics, ied, std::size(ied), vertex_shader_->GetBytecode().GetBufferPointer(), vertex_shader_->GetBytecode().GetBufferSize()));

    }

    std::vector<uint8_t> create_property_memory() {

    }

    float get_float_property(const std::vector<uint8_t>& property_memory, const std::string& name) const {

    }

    void set_float_property(std::vector<uint8_t>& property_memory, const std::string& name, const float& value) const {

    }

    nodec::Vector4f get_vector4_property(const std::vector<uint8_t>& property_memory, const std::string& name) const {

    }

    void set_vector4_property(std::vector<uint8_t>& property_memory, const std::string& name, const nodec::Vector4f& value) const {

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

    std::unique_ptr<InputLayout> input_layout_;
    std::unique_ptr<VertexShader> vertex_shader_;
    std::unique_ptr<PixelShader> pixel_shader_;
};