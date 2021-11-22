#pragma once

#include "ShaderBackend.hpp"

#include <Graphics/Graphics.hpp>
#include <Graphics/ConstantBuffer.hpp>

#include <rendering/resources/material.hpp>

#include <memory>


class MaterialBackend : public rendering::resources::Material {

public:
    MaterialBackend() {

    }

public:
    float get_float_property(const std::string& name) const override {
    
    }

    void set_float_property(const std::string& name, const float& value) override {

    }

    nodec::Vector4f get_vector4_property(const std::string& name) const override {

    }
    void set_vector4_property(const std::string& name, const nodec::Vector4f& value) override {

    }

    TextureEntry get_texture_entry(const std::string& name) const override {

    }
    void set_texture_entry(const std::string& name, const TextureEntry& value) override {

    }

public:
    ConstantBuffer* constant_buffer() {
        return constant_buffer_.get();
    }

    void update_device_memory(Graphics* graphics) {

    }

protected:
    void on_shader_changed() override {
        auto shader_locked = shader();

        auto* shader_backend = static_cast<ShaderBackend*>(shader_locked.get());

    }

private:
    std::unique_ptr<ConstantBuffer> constant_buffer_;
    std::vector<uint8_t> property_memory;


};