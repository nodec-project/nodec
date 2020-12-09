#include "VertexShader.hpp"

VertexShader::VertexShader(Graphics* pGraphics, const void* pShaderBytecode, SIZE_T bytecodeLength)
{
    pGraphics->ThrowIfError(
        pGraphics->GetDevice()->CreateVertexShader(
            pShaderBytecode,
            bytecodeLength,
            nullptr,
            &pVertexShader
        ),
        __FILE__, __LINE__);

}

void VertexShader::Bind(Graphics* pGraphics)
{
    pGraphics->GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0u);
    pGraphics->GetInfoLogger()->Dump(nodec::logging::Level::Debug);
}