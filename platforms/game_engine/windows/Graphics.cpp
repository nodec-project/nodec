#include "Graphics.hpp"

#include <nodec/nodec_exception.hpp>

#include <sstream>

#pragma comment(lib,"d3d11.lib")

Graphics::Graphics(HWND hWnd, int width, int height) :
    width(width),
    height(height)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;                               // the refresh rate 0(denominator)/0(numerator)
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;


    UINT swapCreateFlags = 0u;
    // for checking results of d3d functions.
    HRESULT hr;

    // create device and front/back buffers, and swap chain and rendering context.
    if (FAILED(hr = D3D11CreateDeviceAndSwapChain(
        nullptr,                  // video adapter: nullptr=default adapter
        D3D_DRIVER_TYPE_HARDWARE, // driver type
        nullptr,                  // Needed if D3D_DRIVER_TYPE_SOFTWARE is enabled
        swapCreateFlags,          // flag
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &pSwap,
        &pDevice,
        nullptr,
        &pContext
    )))
    {
        throw HrException(hr, __FILE__, __LINE__);
    }
    pSwap->Present(1u, 0u);
}

Graphics::~Graphics()
{

}

// === Exception ===
Graphics::HrException::HrException(HRESULT hr, const char* file, size_t line) noexcept:
    Exception(file, line)
{

    std::ostringstream oss;
    oss << "[Error Code] 0x" << std::hex << std::uppercase << hr << std::dec
        << " (" << (unsigned long)hr << ")" << std::endl
        << "[Description] " << "" << std::endl;

    message = oss.str();
}
// End Exception ===