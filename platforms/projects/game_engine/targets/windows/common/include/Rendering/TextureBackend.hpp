#pragma once

#include <Graphics/Graphics.hpp>
#include <Graphics/TextureView.hpp>

#include <nodec_rendering/resources/texture.hpp>

#include <memory>

class TextureBackend : public nodec_rendering::resources::Texture {
public:
    TextureBackend(Graphics *gfx, const std::string &path) {
        texture_view_.reset(new TextureView(gfx, path));
    }

public:
    int height() const override {
        return texture_view_->metadata().height;
    }

    int width() const override {
        return texture_view_->metadata().width;
    }

public:
    TextureView *texture_view() {
        return texture_view_.get();
    }

private:
    std::unique_ptr<TextureView> texture_view_;
};