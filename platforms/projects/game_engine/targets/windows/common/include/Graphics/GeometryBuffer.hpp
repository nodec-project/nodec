#pragma once

#include "Graphics.hpp"

class GeometryBuffer {
public:
    GeometryBuffer(Graphics *pGfx, UINT width, UINT height)
        : mWidth{width}, mHeight{height} {
        // Generate the render target textures.
        D3D11_TEXTURE2D_DESC textureDesc{};
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        ThrowIfFailedGfx(
            pGfx->GetDevice().CreateTexture2D(&textureDesc, nullptr, &mpTexture),
            pGfx, __FILE__, __LINE__);

        // Generate the render target views.
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        ThrowIfFailedGfx(
            pGfx->GetDevice().CreateRenderTargetView(mpTexture.Get(), &renderTargetViewDesc, &mpRenderTargetView),
            pGfx, __FILE__, __LINE__);

        // Generate the shader resource views.
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
        shaderResourceViewDesc.Format = textureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;

        ThrowIfFailedGfx(
            pGfx->GetDevice().CreateShaderResourceView(mpTexture.Get(), &shaderResourceViewDesc, &mpShaderResourceView),
            pGfx, __FILE__, __LINE__);
    }

    ID3D11Texture2D& GetTexture() noexcept {
        return *mpTexture.Get();
    }

    ID3D11RenderTargetView &GetRenderTargetView() noexcept {
        return *mpRenderTargetView.Get();
    }

    ID3D11ShaderResourceView &GetShaderResourceView() noexcept {
        return *mpShaderResourceView.Get();
    }

    UINT GetWidth() const noexcept {
        return mWidth;
    }

    UINT GetHeight() const noexcept {
        return mHeight;
    }

private:
    UINT mWidth;
    UINT mHeight;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mpTexture;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mpRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mpShaderResourceView;
};