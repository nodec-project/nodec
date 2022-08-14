#pragma once
#include "Graphics.hpp"

class RasterizerState {
public:
    RasterizerState(Graphics *pGfx, D3D11_CULL_MODE cullMode) {
        D3D11_RASTERIZER_DESC desc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        desc.CullMode = cullMode;
        //desc.FillMode = D3D11_FILL_WIREFRAME;

        ThrowIfFailedGfx(pGfx->GetDevice().CreateRasterizerState(&desc, &mpRasterizerState),
                         pGfx, __FILE__, __LINE__);
    }

    void Bind(Graphics *pGfx) {
        pGfx->GetContext().RSSetState(mpRasterizerState.Get());

        const auto logs = pGfx->GetInfoLogger().Dump();
        if (!logs.empty()) {
            nodec::logging::WarnStream(__FILE__, __LINE__)
                << "[RasterizerState::Bind] >>> DXGI Logs:" << logs;
        }
    }

private:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> mpRasterizerState;
};
