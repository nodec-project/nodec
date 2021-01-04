#include "Graphics/VertexShader.hpp"

VertexShader::VertexShader(Graphics* graphics, 
                           const std::string& path, 
                           const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
                           UINT numElements):
    Shader(path),
    inputLayout(graphics, pInputElementDescs, numElements, bytecode.data(), bytecode.size())
{
    graphics->ThrowIfError(
        graphics->GetDevice()->CreateVertexShader(
            bytecode.data(),
            bytecode.size(),
            nullptr,
            &pVertexShader
        ),
        __FILE__, __LINE__);

}

void VertexShader::Bind(Graphics* graphics)
{
    inputLayout.Bind(graphics);

    graphics->GetInfoLogger()->SetLatest();
    graphics->GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0u);
    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}