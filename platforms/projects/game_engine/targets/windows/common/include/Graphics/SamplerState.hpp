#pragma once
#include "Graphics.hpp"


class SamplerState
{
public:
    enum class Type
    {
        Anisotropic,
        Bilinear,
        Point
    };
public:
    SamplerState(Graphics* pGfx, Type type)
        : type(type)
    {
        D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
        samplerDesc.Filter = [type]()
        {
            switch (type)
            {
            case Type::Anisotropic:
                return D3D11_FILTER_ANISOTROPIC;
            case Type::Point:
                return D3D11_FILTER_MIN_MAG_MIP_POINT;
            case Type::Bilinear:
            default:
                return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            }
        }();
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

        pGfx->ThrowIfError(
            pGfx->GetDevice().CreateSamplerState(&samplerDesc, &mpSamplerState),
            __FILE__, __LINE__);
    }

    void BindVS(Graphics* pGraphics, UINT slot) {
        pGraphics->GetContext().VSSetSamplers(slot, 1, mpSamplerState.GetAddressOf());
        pGraphics->GetInfoLogger().DumpIfAny(nodec::logging::Level::Warn);
    }

    void BindPS(Graphics* pGraphics, UINT slot) {
        pGraphics->GetContext().PSSetSamplers(slot, 1, mpSamplerState.GetAddressOf());
        pGraphics->GetInfoLogger().DumpIfAny(nodec::logging::Level::Warn);
    }

private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> mpSamplerState;
    Type type;
};