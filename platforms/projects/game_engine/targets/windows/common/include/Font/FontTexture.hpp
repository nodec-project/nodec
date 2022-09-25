#pragma once

#include "../Graphics/Graphics.hpp"
#include "FontFace.hpp"

class FontTexture {
public:
    FontTexture(Graphics *pGfx, FT_Face face) {
        D3D11_TEXTURE2D_DESC textureDesc{};
        textureDesc.Format = DXGI_FORMAT_R8_UNORM;
        textureDesc.Width = face->glyph->bitmap.width;
        textureDesc.Height = face->glyph->bitmap.rows;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.ArraySize = 1; // We are making one texture.
        textureDesc.MipLevels = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initData{};
        initData.pSysMem = face->glyph->bitmap.buffer;
        initData.SysMemPitch = face->glyph->bitmap.pitch;
        initData.SysMemSlicePitch = 0;

        ThrowIfFailedGfx(
            pGfx->GetDevice().CreateTexture2D(&textureDesc, &initData, &mpTexture),
            pGfx, __FILE__, __LINE__);

        {
            D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
            desc.Format = textureDesc.Format;
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipLevels = textureDesc.MipLevels;
            desc.Texture2D.MostDetailedMip = 0;
            ThrowIfFailedGfx(
                pGfx->GetDevice().CreateShaderResourceView(mpTexture.Get(), &desc, &mpShaderResourceView),
                pGfx, __FILE__, __LINE__);
        }
    }

    ID3D11ShaderResourceView &GetShaderResourceView() noexcept {
        return *mpShaderResourceView.Get();
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mpTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mpShaderResourceView;
};