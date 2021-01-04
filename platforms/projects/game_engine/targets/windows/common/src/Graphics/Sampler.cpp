#include "Graphics/Sampler.hpp"

Sampler::Sampler(Graphics* graphics, Type type):
    type(type)
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

    graphics->ThrowIfError(graphics->GetDevice()->CreateSamplerState(&samplerDesc, &pSampler),
                           __FILE__, __LINE__);

}

void Sampler::BindVS(Graphics* graphics, UINT slot)
{
    graphics->GetInfoLogger()->SetLatest();
    graphics->GetContext()->VSSetSamplers(slot, 1, pSampler.GetAddressOf());
    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}

void Sampler::BindPS(Graphics* graphics, UINT slot)
{
    graphics->GetInfoLogger()->SetLatest();
    graphics->GetContext()->PSSetSamplers(slot, 1, pSampler.GetAddressOf());
    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}