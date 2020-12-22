#include "Texture.hpp"

Texture::Texture(Graphics* graphics)
{

    // create texture resource
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.MipLevels = 0;
    textureDesc.ArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
    graphics->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);

    // write image data into top mip level



}

void Texture::BindVS(Graphics* graphics, UINT slot)
{
    graphics->GetInfoLogger()->SetLatest();

    graphics->GetContext()->VSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());

    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}

void Texture::BindPS(Graphics* graphics, UINT slot)
{
    graphics->GetInfoLogger()->SetLatest();

    graphics->GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
    
    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}
