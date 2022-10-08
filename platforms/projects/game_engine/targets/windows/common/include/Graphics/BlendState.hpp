#pragma once
#include "Graphics.hpp"

class BlendState {
public:
    static BlendState CreateDefaultBlend(Graphics *pGfx) {
        return {pGfx, CD3D11_BLEND_DESC{CD3D11_DEFAULT{}}};
    }

    static BlendState CreateAlphaBlend(Graphics *pGfx) {
        D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
        rtBlendDesc.BlendEnable = true;
        rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
        rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_MIN;
        rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
        rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
        rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC desc = {};

        for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
            desc.RenderTarget[i] = rtBlendDesc;
        }
        return {pGfx, desc};
    }

    BlendState(Graphics *pGfx, const D3D11_BLEND_DESC &desc) {
        ThrowIfFailedGfx(
            pGfx->GetDevice().CreateBlendState(&desc, &mpBlendState),
            pGfx, __FILE__, __LINE__);
    }

    void Bind(Graphics *pGfx) {
        pGfx->GetContext().OMSetBlendState(mpBlendState.Get(), nullptr, 0xFFFFFFFFu);
    }

private:
    Microsoft::WRL::ComPtr<ID3D11BlendState> mpBlendState;
};