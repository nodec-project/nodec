#pragma once

#include "Graphics.hpp"

#include <nodec/unicode.hpp>

#include <d3dcompiler.h>


class VertexShader {
public:
    VertexShader(Graphics* pGraphics, const std::string& path) {

        pGraphics->ThrowIfError(
            D3DReadFileToBlob(nodec::unicode::utf8to16<std::wstring>(path).c_str(), &mpBytecodeBlob),
            __FILE__, __LINE__
        );

        pGraphics->ThrowIfError(
            pGraphics->GetDevice().CreateVertexShader(
                mpBytecodeBlob->GetBufferPointer(), mpBytecodeBlob->GetBufferSize(),
                nullptr, &mpVertexShader
            ),
            __FILE__, __LINE__
        );
    }

    void Bind(Graphics* pGraphics) {
        pGraphics->GetContext().VSSetShader(mpVertexShader.Get(), nullptr, 0u);
        pGraphics->GetInfoLogger().DumpIfAny(nodec::logging::Level::Warn);
    }

    ID3DBlob& GetBytecode() {
        return *mpBytecodeBlob.Get();
    }

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> mpVertexShader;
    Microsoft::WRL::ComPtr<ID3DBlob> mpBytecodeBlob;

};