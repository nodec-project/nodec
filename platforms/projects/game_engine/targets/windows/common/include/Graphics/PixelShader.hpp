#pragma once

#include "Graphics.hpp"

#include <nodec/unicode.hpp>

#include <d3dcompiler.h>


class PixelShader {
public:
    PixelShader(Graphics* pGraphics, const std::string& path) {
        Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
        
        pGraphics->ThrowIfError(
            D3DReadFileToBlob(nodec::unicode::utf8to16<std::wstring>(path).c_str(), &pBlob),
            __FILE__, __LINE__
        );

        pGraphics->ThrowIfError(
            pGraphics->GetDevice().CreatePixelShader(
                pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
                nullptr, &mpPixelShader
            ),
            __FILE__, __LINE__
        );
    }

    void Bind(Graphics* pGraphics) {
        pGraphics->GetContext().PSSetShader(mpPixelShader.Get(), nullptr, 0u);
        pGraphics->GetInfoLogger().DumpIfAny(nodec::logging::Level::Warn);
    }

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mpPixelShader;
};