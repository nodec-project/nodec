#pragma once

#include "DxgiInfoLogger.hpp"

#include <nodec/nodec_exception.hpp>
#include <nodec/macros.hpp>

#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

class Graphics
{
public:
    class Exception : public nodec::NodecException
    {
        using NodecException::NodecException;
    };

    class HrException : public Exception
    {
    public:
        HrException(HRESULT hr, const char* file, size_t line) noexcept;
        const char* type() const noexcept override { return "Graphics::HrException"; }

    };

    class DeviceRemovedException : public HrException
    {
        using HrException::HrException;
        const char* type() const noexcept override { return "Graphics::DeviceRemovedException";  }

    };

public:
    Graphics(HWND hWnd, int width, int height);
    ~Graphics();

    void BeginFrame() noexcept;
    void EndFrame();
    void DrawIndexed(UINT count);

    void ThrowIfError(HRESULT hr, const char* file, size_t line);
    void DrawTestTriangle();

    ID3D11Device* GetDevice() noexcept;
    ID3D11DeviceContext* GetContext() noexcept;
    DxgiInfoLogger* GetInfoLogger() noexcept;

private:
    UINT width;
    UINT height;

    DxgiInfoLogger infoLogger;
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>pDSV;
    
private:
    NODEC_DISABLE_COPY(Graphics);
};