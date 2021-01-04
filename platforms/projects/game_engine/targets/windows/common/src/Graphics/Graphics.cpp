#include "Graphics/Graphics.hpp"

#include <d3dcompiler.h>

#include <nodec/nodec_exception.hpp>


#include <sstream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")


namespace wrl = Microsoft::WRL;
//namespace dx = DirectX;

void Graphics::ThrowIfError(HRESULT hr, const char* file, size_t line)
{
    if (FAILED(hr))
    {
        infoLogger.Dump(nodec::logging::Level::Error);
        throw HrException(hr, file, line);
    }
}

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
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;

    // for checking results of d3d functions.
    infoLogger.SetLatest();

    // create device and front/back buffers, and swap chain and rendering context.
    ThrowIfError(D3D11CreateDeviceAndSwapChain(
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
    ), __FILE__, __LINE__);

    // gain access to texture subresource in swap chain (back buffer)
    wrl::ComPtr<ID3D11Texture2D> pBackBuffer;

    ThrowIfError(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer), __FILE__, __LINE__);
    ThrowIfError(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget), __FILE__, __LINE__);

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    wrl::ComPtr<ID3D11DepthStencilState> pDSState;
    ThrowIfError(pDevice->CreateDepthStencilState(&dsDesc, &pDSState), __FILE__, __LINE__);

    // bind depth state
    pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

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
    ThrowIfError(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil), __FILE__, __LINE__);

    // create view of depth stensil texture
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    ThrowIfError(
        pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDSV),
        __FILE__, __LINE__
    );

    // bind depth stencil view to OM
    pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

    //pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    pContext->RSSetViewports(1u, &vp);

    infoLogger.Dump(nodec::logging::Level::Info);
    nodec::logging::InfoStream(__FILE__, __LINE__) << "[Graphics] >>> Successfully initialized." << std::flush;
}

Graphics::~Graphics()
{
    infoLogger.DumpIfAny(nodec::logging::Level::Info);
    nodec::logging::InfoStream(__FILE__, __LINE__) << "[Graphics] >>> End Graphics." << std::flush;
}

ID3D11Device* Graphics::GetDevice() noexcept { return pDevice.Get(); }
ID3D11DeviceContext* Graphics::GetContext() noexcept { return pContext.Get(); }
DxgiInfoLogger* Graphics::GetInfoLogger() noexcept { return &infoLogger; }

UINT Graphics::GetWidth() noexcept { return width; }
UINT Graphics::GetHeight() noexcept { return height; }


void Graphics::BeginFrame() noexcept
{
    const float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };

    pContext->ClearRenderTargetView(pTarget.Get(), color);
    pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::EndFrame()
{
    HRESULT hr;
    infoLogger.SetLatest();
    if (FAILED(hr = pSwap->Present(1u, 0u)))
    {
        infoLogger.Dump(nodec::logging::Level::Fatal);
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw DeviceRemovedException(pDevice->GetDeviceRemovedReason(), __FILE__, __LINE__);
        }
        else
        {
            throw HrException(hr, __FILE__, __LINE__);
        }
    }
}

void Graphics::DrawIndexed(UINT count)
{
    infoLogger.SetLatest();
    pContext->DrawIndexed(count, 0u, 0u);
    infoLogger.DumpIfAny(nodec::logging::Level::Warn);
}

void Graphics::DrawTestTriangle()
{
    namespace wrl = Microsoft::WRL;

    struct Vertex
    {
        float x;
        float y;
    };

    const Vertex vertices[] = {
        {0.0f, 0.5f},
        {0.5f, -0.5f},
        {-0.5f, -0.5f}
    };
    wrl::ComPtr<ID3D11Buffer> pVertexBuffer;

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;
    ThrowIfError(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer), __FILE__, __LINE__);

    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

    // For file reading
    wrl::ComPtr<ID3DBlob> pBlob;

    // create pixel shader
    wrl::ComPtr<ID3D11PixelShader> pPixelShader;
    D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
    pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);

    // bind pixel shader
    pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);


    // create vertex shader
    wrl::ComPtr<ID3D11VertexShader> pVertexShader;
    D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
    pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);

    pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);


    // input (vertex) layout (2d position only) 
    wrl::ComPtr<ID3D11InputLayout> pInputLayout;
    const D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    pDevice->CreateInputLayout(
        ied, std::size(ied),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &pInputLayout);

    // bind vertex layout
    pContext->IASetInputLayout(pInputLayout.Get());

    // bind render target
    pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);


    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pContext->Draw((UINT)std::size(vertices), 0u);

    //infoLogger.Dump(nodec::logging::Level::Debug);
}
// === Exception ===
Graphics::HrException::HrException(HRESULT hr, const char* file, size_t line) noexcept :
    Exception(file, line)
{

    std::ostringstream oss;
    oss << "[Error Code] 0x" << std::hex << std::uppercase << hr << std::dec
        << " (" << (unsigned long)hr << ")" << std::endl;

    message = oss.str();
}


// End Exception ===