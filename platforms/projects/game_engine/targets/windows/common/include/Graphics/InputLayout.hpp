#pragma once

#include "Graphics.hpp"

class InputLayout
{
public:
    InputLayout(Graphics* graphics,
                const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
                UINT numElements,
                const void* pShaderBytecode,
                SIZE_T bytecodeLength);

    void Bind(Graphics* graphics);

private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};