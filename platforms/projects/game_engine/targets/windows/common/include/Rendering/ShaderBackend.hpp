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

        for (auto& property : float_properties_) {
            float_property_offsets_[property.name] = property_memory_prototype.size();
            append_property(property_memory_prototype, property.default_value);
        }
        align_memory(property_memory_prototype);

        for (auto& property : vector4_properties_) {
            vector4_property_offsets_[property.name] = property_memory_prototype.size();
            append_property(property_memory_prototype, property.default_value);
        }
        align_memory(property_memory_prototype);

        for (int slot = 0; slot < texture_entries_.size(); ++slot) {
            auto& entry = texture_entries_[slot];
            texture_entry_slots_[entry.name] = slot;
        }
    }

    std::vector<uint8_t> create_property_memory() const {
        return property_memory_prototype;
    }

    float get_float_property(const std::vector<uint8_t>& property_memory, const std::string& name) const {
        auto offset = float_property_offsets_.at(name);
        return *get_property_ptr<float>(property_memory, offset);
    }

    void set_float_property(std::vector<uint8_t>& property_memory, const std::string& name, const float& value) const {
        auto offset = float_property_offsets_.at(name);
        auto* v = get_property_ptr<float>(property_memory, offset);
        *v = value;
    }

    nodec::Vector4f get_vector4_property(const std::vector<uint8_t>& property_memory, const std::string& name) const {
        using namespace nodec;
        auto offset = vector4_property_offsets_.at(name);
        return *get_property_ptr<Vector4f>(property_memory, offset);
    }

    void set_vector4_property(std::vector<uint8_t>& property_memory, const std::string& name, const nodec::Vector4f& value) const {
        using namespace nodec;
        auto offset = vector4_property_offsets_.at(name);
        auto* v = get_property_ptr<Vector4f>(property_memory, offset);
        *v = value;
    }

    int get_texture_slot(const std::string& name) const {
        return texture_entry_slots_.at(name);
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

    InputLayout* input_layout() {
        return input_layout_.get();
    }

    VertexShader* vertex_shader() {
        return vertex_shader_.get();
    }

    PixelShader* pixel_shader() {
        return pixel_shader_.get();
    }

private:
    template<typename T>
    static void append_property(std::vector<uint8_t>& memory, const T &value) {
        // <https://stackoverrun.com/ja/q/3787586>
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&value);
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            memory.push_back(p[i]);
        }
    }

    template<typename T>
    static T* get_property_ptr(std::vector<uint8_t>& memory, const int offset) {
        // <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0007r0.html>
        return const_cast<T*>(get_property_ptr<T>(const_cast<std::add_const_t<decltype(memory)>&>(memory), offset));
    }

    template<typename T>
    static const T* get_property_ptr(const std::vector<uint8_t>& memory, const int offset) {
        auto end = offset + sizeof(T);
        if (end > memory.size()) {
            throw std::out_of_range("Exceed property memory boundary.");
        }

        return reinterpret_cast<const T*>(&memory[offset]);
    }

    static void align_memory(std::vector<uint8_t>& memory) {
        // Packing Rules for Constant Variables
        // <https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules>
        constexpr size_t alignment_size = 16;
        size_t aligned = (memory.size() + (alignment_size - 1)) & ~(alignment_size - 1);

        // add padding so that size will be a multiple of 16.
        while (memory.size() < aligned)
        {
            memory.push_back(0x00);
        }
    }


private:
    std::vector<FloatProperty> float_properties_;
    std::unordered_map<std::string, int> float_property_offsets_;

    std::vector<Vector4Property> vector4_properties_;
    std::unordered_map<std::string, int> vector4_property_offsets_;

    std::vector<TextureEntry> texture_entries_;
    std::unordered_map<std::string, int> texture_entry_slots_;

    std::vector<uint8_t> property_memory_prototype;

    std::unique_ptr<InputLayout> input_layout_;
    std::unique_ptr<VertexShader> vertex_shader_;
    std::unique_ptr<PixelShader> pixel_shader_;
};