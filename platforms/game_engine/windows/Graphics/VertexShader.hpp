#pragma once

#include "Shader.hpp"
#include "InputLayout.hpp"
#include "Graphics.hpp"

#include <nodec/macros.hpp>

class VertexShader : public Shader
{
public:
    VertexShader(Graphics* graphics,
                 const std::string& path,
                 const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
                 UINT numElements);

    void Bind(Graphics* graphics);

private:
    InputLayout inputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;

};