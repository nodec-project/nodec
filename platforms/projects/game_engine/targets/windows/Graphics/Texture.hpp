#pragma once

#include "Graphics.hpp"

class Texture
{
public:
    Texture(Graphics* graphics);

    void BindVS(Graphics* graphics, UINT slot);
    void BindPS(Graphics* graphics, UINT slot);

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;

private:
    NODEC_DISABLE_COPY(Texture);
};