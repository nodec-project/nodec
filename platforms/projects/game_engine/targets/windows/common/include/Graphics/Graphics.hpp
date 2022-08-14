#pragma once

#include "DxgiInfoLogger.hpp"

#include <nodec/formatter.hpp>
#include <nodec/macros.hpp>

// Prevent to define min/max macro in windows api.
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

#include <stdexcept>

class Graphics {
public:
    Graphics(HWND hWnd, int width, int height);
    ~Graphics();

    void BeginFrame() noexcept;
    void EndFrame();
    void DrawIndexed(UINT count);

    ID3D11Device &GetDevice() noexcept {
        return *mpDevice.Get();
    }
    ID3D11DeviceContext &GetContext() noexcept {
        return *mpContext.Get();
    }
    DxgiInfoLogger &GetInfoLogger() noexcept {
        return mInfoLogger;
    };

    UINT GetWidth() const noexcept {
        return mWidth;
    };
    UINT GetHeight() const noexcept {
        return mHeight;
    };

private:
    UINT mWidth;
    UINT mHeight;

    DxgiInfoLogger mInfoLogger;
    Microsoft::WRL::ComPtr<ID3D11Device> mpDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> mpSwap;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> mpContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mpTarget;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mpDSV;

private:
    NODEC_DISABLE_COPY(Graphics);
};

inline void ThrowIfFailedGfx(const std::string &type, HRESULT hr, Graphics *pGfx, const char *file, size_t line) {
    using namespace nodec;
    if (FAILED(hr)) {
        const auto logs = pGfx->GetInfoLogger().Dump();

        throw std::runtime_error(
            ErrorFormatter<std::runtime_error>(file, line)
            << "GraphicsError::" << type << " [Error Code] 0x" << std::hex << std::uppercase << hr << std::dec
            << " (" << (unsigned long)hr << ")\n"
            << "Last Dxgi Debug Logs:\n"
            << logs);
    }
}

inline void ThrowIfFailedGfx(HRESULT hr, Graphics *pGfx, const char *file, size_t line) {
    ThrowIfFailedGfx("", hr, pGfx, file, line);
}