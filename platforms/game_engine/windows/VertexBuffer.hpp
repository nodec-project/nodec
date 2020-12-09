#pragma once

#include "Graphics.hpp"

/**
* vertices in device memory
*/
class VertexBuffer
{
public:
    VertexBuffer(Graphics* pGraphics, UINT sizeBytes, UINT strideBytes, void* pSysMem);

    void Bind(Graphics* pGraphics);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
    UINT sizeBytes;
    UINT strideBytes;

private:
    NODEC_DISABLE_COPY(VertexBuffer);
};