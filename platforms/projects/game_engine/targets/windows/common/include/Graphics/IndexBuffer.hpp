#pragma once

#include "Graphics.hpp"

class IndexBuffer
{
public:
    IndexBuffer(Graphics* pGraphics, UINT size, const void* pSysMem);
    void Bind(Graphics* pGraphics);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;

private:
    NODEC_DISABLE_COPY(IndexBuffer);
};