#pragma once

#include "Graphics.hpp"

#include <nodec/macros.hpp>

class VertexShader
{
public:
    VertexShader(Graphics* pGraphics, const void* pShaderBytecode, SIZE_T bytecodeLength);

    void Bind(Graphics* pGraphics);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;

private:
    NODEC_DISABLE_COPY(VertexShader);
};