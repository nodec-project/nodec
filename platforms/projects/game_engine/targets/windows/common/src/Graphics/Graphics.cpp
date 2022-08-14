#include "Graphics/Graphics.hpp"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <d3dcompiler.h>

#include <sstream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;
// namespace dx = DirectX;

// About usage of D3D11
//  * https://docs.microsoft.com/en-us/windows/win32/direct3d11/dx-graphics-overviews

Graphics::Graphics(HWND hWnd, int width, int height)
    : mWidth(width), mHeight(height) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0; // the refresh rate 0(denominator)/0(numerator)
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
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;

    // // for checking results of d3d functions.
    // mInfoLogger.SetLatest();

    // create device and front/back buffers, and swap chain and rendering context.
    ThrowIfFailedGfx(D3D11CreateDeviceAndSwapChain(
                         nullptr,                  // video adapter: nullptr=default adapter
                         D3D_DRIVER_TYPE_HARDWARE, // driver type
                         nullptr,                  // Needed if D3D_DRIVER_TYPE_SOFTWARE is enabled
                         swapCreateFlags,          // flag
                         nullptr,
                         0,
                         D3D11_SDK_VERSION,
                         &sd,
                         &mpSwap,
                         &mpDevice,
                         nullptr,
                         &mpContext),
                     this, __FILE__, __LINE__);

    // gain access to texture subresource in swap chain (back buffer)
    wrl::ComPtr<ID3D11Texture2D> pBackBuffer;

    ThrowIfFailedGfx(mpSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer), this, __FILE__, __LINE__);
    ThrowIfFailedGfx(mpDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &mpTarget), this, __FILE__, __LINE__);

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    wrl::ComPtr<ID3D11DepthStencilState> pDSState;
    ThrowIfFailedGfx(mpDevice->CreateDepthStencilState(&dsDesc, &pDSState), this, __FILE__, __LINE__);

    // bind depth state
    mpContext->OMSetDepthStencilState(pDSState.Get(), 1u);

    // create depth stensil texture
    wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ThrowIfFailedGfx(mpDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil), this, __FILE__, __LINE__);

    // create view of depth stensil texture
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    ThrowIfFailedGfx(
        mpDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &mpDSV),
        this, __FILE__, __LINE__);

    // bind depth stencil view to OM
    mpContext->OMSetRenderTargets(1u, mpTarget.GetAddressOf(), mpDSV.Get());

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    mpContext->RSSetViewports(1u, &vp);

    // init imgui d3d impl
    ImGui_ImplDX11_Init(mpDevice.Get(), mpContext.Get());

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[Graphics] >>> DXGI Debug Logs:\n"
        << mInfoLogger.Dump();

    nodec::logging::InfoStream(__FILE__, __LINE__) << "[Graphics] >>> Successfully initialized." << std::flush;
}

Graphics::~Graphics() {
    ImGui_ImplDX11_Shutdown();

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[Graphics] >>> DXGI Debug Logs:\n"
        << mInfoLogger.Dump();
    nodec::logging::InfoStream(__FILE__, __LINE__) << "[Graphics] >>> End Graphics." << std::flush;
}

void Graphics::BeginFrame() noexcept {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    mpContext->OMSetRenderTargets(1u, mpTarget.GetAddressOf(), mpDSV.Get());

    const float color[] = {0.1f, 0.1f, 0.1f, 1.0f};

    mpContext->ClearRenderTargetView(mpTarget.Get(), color);
    mpContext->ClearDepthStencilView(mpDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::EndFrame() {
    // auto& io = ImGui::GetIO();
    // nodec::logging::InfoStream(__FILE__, __LINE__) << io.DisplayFramebufferScale.x << ", " << io.DisplayFramebufferScale.y;

    ImGui::Render();

    mpContext->OMSetRenderTargets(1u, mpTarget.GetAddressOf(), mpDSV.Get());

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    HRESULT hr;
    // mInfoLogger.SetLatest();
    if (FAILED(hr = mpSwap->Present(1u, 0u))) {
        if (hr == DXGI_ERROR_DEVICE_REMOVED) {
            ThrowIfFailedGfx("DeviceRemoved", hr, this, __FILE__, __LINE__);
        } else {
            ThrowIfFailedGfx(hr, this, __FILE__, __LINE__);
        }
    }
}

void Graphics::DrawIndexed(UINT count) {
    mpContext->DrawIndexed(count, 0u, 0u);
    const auto logs = mInfoLogger.Dump();
    if (!logs.empty()) {
        nodec::logging::WarnStream(__FILE__, __LINE__)
            << "[Graphics::DrawIndexed] >>> DXGI debug having messages:\n"
            << logs;
    }
}
