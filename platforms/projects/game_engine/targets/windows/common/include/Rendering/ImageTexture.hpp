#pragma once

#include "TextureBackend.hpp"

#include <nodec/unicode.hpp>

#include <DirectXTex.h>

class ImageTexture : public TextureBackend {

public:
    ImageTexture(Graphics* gfx, const std::string& path) {

        using namespace DirectX;

        enum class ImageType
        {
            TGA,
            WIC
        };

        ImageType image_type = ImageType::WIC;

        auto extention_pos = path.find_last_of('.');
        if (extention_pos != std::string::npos)
        {
            auto extention = path.substr(extention_pos);

            if (extention == ".tga" || extention == ".TGA")
            {
                image_type = ImageType::TGA;
            }
        }

        std::wstring path_wide = nodec::unicode::utf8to16<std::wstring>(path);

        ScratchImage image;
        switch (image_type)
        {
        case ImageType::TGA:
            ThrowIfFailedGfx(
                LoadFromTGAFile(path_wide.c_str(), &metadata_, image),
                gfx, __FILE__, __LINE__);
            break;

        default:
        case ImageType::WIC:
            ThrowIfFailedGfx(
                LoadFromWICFile(path_wide.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, &metadata_, image),
                gfx, __FILE__, __LINE__);
            break;
        }

        // create the resource view on the texture
        ThrowIfFailedGfx(
            CreateShaderResourceView(&gfx->GetDevice(), image.GetImages(), image.GetImageCount(), metadata_, &shader_resource_view_),
            gfx, __FILE__, __LINE__);

        initialize(shader_resource_view_.Get(), metadata_.width, metadata_.height);
    }

private:
    DirectX::TexMetadata metadata_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view_;
};