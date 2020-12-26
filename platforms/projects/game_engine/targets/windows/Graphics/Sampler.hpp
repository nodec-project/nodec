#pragma once
#include "Graphics.hpp"

class Sampler
{
public:
    enum class Type
    {
        Anisotropic,
        Bilinear,
        Point
    };
public:
    Sampler(Graphics* graphics, Type type);

    void BindVS(Graphics* graphics, UINT slot);
    void BindPS(Graphics* graphics, UINT slot);

private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
    Type type;
};