#pragma once

#include "../Rendering/TextureBackend.hpp"

#include "FontFace.hpp"

#include <cassert>

class FontTexture : public TextureBackend {
public:
    FontTexture(Graphics *gfx, FT_Face face) {
        assert(face->glyph->bitmap.buffer != nullptr);

        D3D11_TEXTURE2D_DESC texture_desc{};
        texture_desc.Format = DXGI_FORMAT_R8_UNORM;
        texture_desc.Width = face->glyph->bitmap.width;
        texture_desc.Height = face->glyph->bitmap.rows;
        texture_desc.SampleDesc.Count = 1;
        texture_desc.ArraySize = 1; // We are making one texture.
        texture_desc.MipLevels = 1;
        texture_desc.Usage = D3D11_USAGE_DEFAULT;
        texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA init_data{};
        init_data.pSysMem = face->glyph->bitmap.buffer;
        init_data.SysMemPitch = face->glyph->bitmap.pitch;
        init_data.SysMemSlicePitch = 0;

        ThrowIfFailedGfx(
            gfx->GetDevice().CreateTexture2D(&texture_desc, &init_data, &texture_),
            gfx, __FILE__, __LINE__);

        {
            D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
            desc.Format = texture_desc.Format;
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipLevels = texture_desc.MipLevels;
            desc.Texture2D.MostDetailedMip = 0;
            ThrowIfFailedGfx(
                gfx->GetDevice().CreateShaderResourceView(texture_.Get(), &desc, &shader_resource_view_),
                gfx, __FILE__, __LINE__);
        }

        initialize(shader_resource_view_.Get(), face->glyph->bitmap.width, face->glyph->bitmap.rows);
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view_;
};