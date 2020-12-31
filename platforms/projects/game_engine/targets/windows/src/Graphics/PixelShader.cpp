#include "Graphics/PixelShader.hpp"

PixelShader::PixelShader(Graphics* graphics,
                         const std::string& path) :
    Shader(path)
{
    graphics->ThrowIfError(
        graphics->GetDevice()->CreatePixelShader(
            bytecode.data(),
            bytecode.size(),
            nullptr, &pPixelShader),
        __FILE__, __LINE__
    );
}

void PixelShader::Bind(Graphics* graphics)
{
    graphics->GetInfoLogger()->SetLatest();
    graphics->GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0u);
    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}