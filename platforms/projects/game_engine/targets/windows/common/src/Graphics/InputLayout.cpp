#include "Graphics/InputLayout.hpp"

InputLayout::InputLayout(Graphics* graphics,
                         const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
                         UINT numElements,
                         const void* pShaderBytecode,
                         SIZE_T bytecodeLength)
{
    graphics->ThrowIfError(
        graphics->GetDevice()->CreateInputLayout(
            pInputElementDescs,
            numElements,
            pShaderBytecode,
            bytecodeLength,
            &pInputLayout
        ),
        __FILE__, __LINE__
    );
}

void InputLayout::Bind(Graphics* graphics)
{
    graphics->GetInfoLogger()->SetLatest();
    graphics->GetContext()->IASetInputLayout(pInputLayout.Get());
    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}