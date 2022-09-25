#pragma once

#include <Graphics/Graphics.hpp>

#include <nodec_rendering/resources/texture.hpp>

#include <memory>

class TextureBackend : public nodec_rendering::resources::Texture {

public:
    TextureBackend() {}

    ID3D11ShaderResourceView& shader_resource_view() {
        return *shader_resource_view_;
    }

    int height() const noexcept override {
        return height_;
    }

    int width() const noexcept override {
        return width_;
    }

protected:
    // NOTE: Why not use virtual functions for each width, height...
    //  We are worried about performance issues with polymorphic call.
    void initialize(ID3D11ShaderResourceView *resource_view, int width, int height) {
        shader_resource_view_ = resource_view;
        width_ = width;
        height_ = height;
    }

private:
    ID3D11ShaderResourceView* shader_resource_view_{nullptr};
    int width_;
    int height_;
};