#pragma once

#include "Graphics.hpp"

#include <nodec/unicode.hpp>

#include <DirectXTex.h>


class TextureView
{
public:
    TextureView(Graphics* pGfx, const std::string& path) {
        using namespace DirectX;

        enum class ImageType
        {
            TGA,
            WIC
        };

        ImageType imageType = ImageType::WIC;

        auto extention_pos = path.find_last_of('.');
        if (extention_pos != std::string::npos)
        {
            auto extention = path.substr(extention_pos);

            if (extention == ".tga" || extention == ".TGA")
            {
                imageType = ImageType::TGA;
            }
        }

        std::wstring pathW = nodec::unicode::utf8to16<std::wstring>(path);

        ScratchImage image;
        switch (imageType)
        {
        case ImageType::TGA:
            ThrowIfFailedGfx(
                LoadFromTGAFile(pathW.c_str(), &mMetadata, image),
                pGfx, __FILE__, __LINE__);
            break;

        default:
        case ImageType::WIC:
            ThrowIfFailedGfx(
                LoadFromWICFile(pathW.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, &mMetadata, image),
                pGfx, __FILE__, __LINE__);
            break;
        }

        // create the resource view on the texture
        ThrowIfFailedGfx(
            CreateShaderResourceView(&pGfx->GetDevice(), image.GetImages(), image.GetImageCount(), mMetadata, &mpTextureView),
            pGfx, __FILE__, __LINE__);
    }

    void BindVS(Graphics* pGfx, UINT slot) {
        pGfx->GetContext().VSSetShaderResources(slot, 1u, mpTextureView.GetAddressOf());

        const auto logs = pGfx->GetInfoLogger().Dump();
        if (!logs.empty()) {
            nodec::logging::WarnStream(__FILE__, __LINE__)
                << "[TextureView::BindVS] >>> DXGI Logs:"
                << logs;
        }
    }

    void BindPS(Graphics* pGfx, UINT slot) {
        pGfx->GetContext().PSSetShaderResources(slot, 1u, mpTextureView.GetAddressOf());

        const auto logs = pGfx->GetInfoLogger().Dump();
        if (!logs.empty()) {
            nodec::logging::WarnStream(__FILE__, __LINE__)
                << "[TextureView::BindPS] >>> DXGI Logs:"
                << logs;
        }
    }

    const DirectX::TexMetadata& metadata() const noexcept {
        return mMetadata;
    }

private:
    DirectX::TexMetadata mMetadata;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mpTextureView;

private:
    NODEC_DISABLE_COPY(TextureView)
};