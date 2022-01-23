#pragma once

#include "Graphics.hpp"

#include <nodec/unicode.hpp>

#include <d3dcompiler.h>


class PixelShader {
public:
    PixelShader(Graphics* pGraphics, const std::string& path) {
        Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

        ThrowIfFailedGfx(
            D3DReadFileToBlob(nodec::unicode::utf8to16<std::wstring>(path).c_str(), &pBlob),
            pGraphics, __FILE__, __LINE__
        );

        ThrowIfFailedGfx(
            pGraphics->GetDevice().CreatePixelShader(
                pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
                nullptr, &mpPixelShader
            ),
            pGraphics, __FILE__, __LINE__
        );
    }

    void Bind(Graphics* pGraphics) {
        pGraphics->GetContext().PSSetShader(mpPixelShader.Get(), nullptr, 0u);

        const auto logs = pGraphics->GetInfoLogger().Dump();
        if (!logs.empty()) {
            nodec::logging::WarnStream(__FILE__, __LINE__)
                << "[PixelShader::Bind] >>> DXGI Logs:"
                << logs;
        }
    }

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mpPixelShader;
};