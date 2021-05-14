#pragma once

#include "DxgiInfoLogger.hpp"

#include <nodec/error_formatter.hpp>
#include <nodec/macros.hpp>

#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

#include <stdexcept>


class Graphics {
public:
    class HrException : public std::runtime_error {
    public:
        /*static void Throw(HRESULT hr, const char* file, size_t line) {
            throw HrException(hr, typeid(HrException).name(), file, line);
        }*/

        HrException(HRESULT hr, const char* file, size_t line)
            : HrException(hr, typeid(HrException).name(), file, line) {
        };

        HrException(HRESULT hr, const char* type, const char* file, size_t line) noexcept
            : errorCode(hr)
            , runtime_error(nodec::error_fomatter::with_type_file_line(
                nodec::Formatter() 
                << "[Error Code] 0x" << std::hex << std::uppercase << hr << std::dec
                << " (" << (unsigned long)hr << ")",
                type, file, line
            )) {
        }

    public:
        HRESULT ErrorCode() const noexcept { return errorCode; }

    private:
        const HRESULT errorCode;
    };

    class DeviceRemovedException : public HrException {
    public:
        DeviceRemovedException(HRESULT hr, const char* file, size_t line)
            : HrException(hr, typeid(DeviceRemovedException).name(), file, line) {

        }
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

    UINT GetWidth() noexcept;
    UINT GetHeight() noexcept;

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