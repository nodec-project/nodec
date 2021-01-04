#pragma once

#include "Graphics.hpp"
#include "Shader.hpp"

class PixelShader : public Shader
{
public:
    PixelShader(Graphics* graphics,
                const std::string& path);

    void Bind(Graphics* graphics);
private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};