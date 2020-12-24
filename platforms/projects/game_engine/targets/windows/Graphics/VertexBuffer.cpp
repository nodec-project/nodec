#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer(Graphics* pGraphics, UINT sizeBytes, UINT strideBytes, const void* pSysMem):
    sizeBytes(sizeBytes),
    strideBytes(strideBytes)
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = sizeBytes;
    bd.StructureByteStride = strideBytes;
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = pSysMem;
    pGraphics->ThrowIfError(
        pGraphics->GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer),
        __FILE__, __LINE__);
}

void VertexBuffer::Bind(Graphics* pGraphics)
{
    const UINT offset = 0u;

    pGraphics->GetInfoLogger()->SetLatest();
    pGraphics->GetContext()->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &strideBytes, &offset);
    pGraphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}