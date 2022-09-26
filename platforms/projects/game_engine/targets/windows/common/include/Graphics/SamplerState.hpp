#pragma once
#include "Graphics.hpp"

#include <nodec_rendering/sampler.hpp>

class SamplerState {
public:
    static SamplerState Create(Graphics *pGfx, const nodec_rendering::Sampler &sampler) {
        using namespace nodec_rendering;
        D3D11_SAMPLER_DESC desc = CD3D11_SAMPLER_DESC{CD3D11_DEFAULT{}};

        desc.Filter = [&]() {
            switch (sampler.filter_mode) {
            case Sampler::FilterMode::Anisotropic:
                return D3D11_FILTER_ANISOTROPIC;
            case Sampler::FilterMode::Point:
                return D3D11_FILTER_MIN_MAG_MIP_POINT;
            case Sampler::FilterMode::Bilinear:
            default:
                return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            }
        }();

        auto wrapMode = [&]() {
            switch (sampler.wrap_mode) {
            default:
            case Sampler::WrapMode::Wrap:
                return D3D11_TEXTURE_ADDRESS_WRAP;
            case Sampler::WrapMode::Clamp:
                return D3D11_TEXTURE_ADDRESS_CLAMP;
            }
        }();
        desc.AddressU = wrapMode;
        desc.AddressV = wrapMode;
        desc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
        return {pGfx, desc};
    }

    SamplerState(Graphics *pGfx, const D3D11_SAMPLER_DESC &desc) {
        ThrowIfFailedGfx(
            pGfx->GetDevice().CreateSamplerState(&desc, &mpSamplerState),
            pGfx, __FILE__, __LINE__);
    }

    void BindVS(Graphics *pGraphics, UINT slot) {
        pGraphics->GetContext().VSSetSamplers(slot, 1, mpSamplerState.GetAddressOf());

        const auto logs = pGraphics->GetInfoLogger().Dump();
        if (!logs.empty()) {
            nodec::logging::WarnStream(__FILE__, __LINE__)
                << "[SamplerState::BindVS] >>> DXGI Logs:"
                << logs;
        }
    }

    void BindPS(Graphics *pGraphics, UINT slot) {
        pGraphics->GetContext().PSSetSamplers(slot, 1, mpSamplerState.GetAddressOf());

        const auto logs = pGraphics->GetInfoLogger().Dump();
        if (!logs.empty()) {
            nodec::logging::WarnStream(__FILE__, __LINE__)
                << "[SamplerState::BindPS] >>> DXGI Logs:"
                << logs;
        }
    }

private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> mpSamplerState;
};