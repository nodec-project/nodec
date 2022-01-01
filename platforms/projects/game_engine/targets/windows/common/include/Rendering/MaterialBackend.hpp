#pragma once

#include "ShaderBackend.hpp"

#include <Graphics/Graphics.hpp>
#include <Graphics/ConstantBuffer.hpp>

#include <rendering/resources/material.hpp>

#include <memory>


class MaterialBackend : public rendering::resources::Material {

public:
    MaterialBackend(Graphics* gfx)
        : gfx_(gfx) {

    }

private:
    ShaderBackend* shader_backend_assured() const {
        auto shader_locked = shader();
        auto* shader_backend = static_cast<ShaderBackend*>(shader_locked.get());
        if (!shader_backend) {
            throw std::runtime_error("No shader");
        }
        return shader_backend;
    }
public:
    float get_float_property(const std::string& name) const override {
        auto* shader_backend = shader_backend_assured();
        return shader_backend->get_float_property(property_memory_, name);
    }

    void set_float_property(const std::string& name, const float& value) override {
        auto* shader_backend = shader_backend_assured();
        shader_backend->set_float_property(property_memory_, name, value);
        dirty_ = true;
    }

    nodec::Vector4f get_vector4_property(const std::string& name) const override {
        auto* shader_backend = shader_backend_assured();
        return shader_backend->get_vector4_property(property_memory_, name);
    }
    void set_vector4_property(const std::string& name, const nodec::Vector4f& value) override {
        auto* shader_backend = shader_backend_assured();
        shader_backend->set_vector4_property(property_memory_, name, value);
        dirty_ = true;
    }

    TextureEntry get_texture_entry(const std::string& name) const override {
        auto* shader_backend = shader_backend_assured();
        auto slot = shader_backend->get_texture_slot(name);
        return texture_entries_[slot];
    }

    void set_texture_entry(const std::string& name, const TextureEntry& value) override {
        auto* shader_backend = shader_backend_assured();
        auto slot = shader_backend->get_texture_slot(name);
        texture_entries_[slot] = value;
    }

public:
    ConstantBuffer* constant_buffer() {
        return constant_buffer_.get();
    }

    void update_device_memory() {
        if (!dirty_ && !constant_buffer_) {
            return;
        }

        constant_buffer_->Update(gfx_, property_memory_.data());
        dirty_ = false;
    }

    const auto& texture_entries() const { return texture_entries_; }

protected:
    void on_shader_changed() override {
        constant_buffer_.reset();
        property_memory_.clear();
        texture_entries_.clear();

        auto shader_locked = shader();
        auto* shader_backend = static_cast<ShaderBackend*>(shader_locked.get());
        if (!shader_backend) {
            return;
        }
        property_memory_ = shader_backend->create_property_memory();

        constant_buffer_.reset(new ConstantBuffer(gfx_, property_memory_.size(), property_memory_.data()));

        texture_entries_.resize(shader_backend->texture_entries().size());

        dirty_ = true;
    }

private:
    Graphics* gfx_;

    std::unique_ptr<ConstantBuffer> constant_buffer_;
    std::vector<uint8_t> property_memory_;
    std::vector<TextureEntry> texture_entries_;
    bool dirty_{ true };
};