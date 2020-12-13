#pragma once

#include "Graphics.hpp"

class ConstantBuffer
{
public:
    ConstantBuffer(Graphics* graphics, UINT sizeBytes, const void* pSysMem);

    void Update(Graphics* graphics, const void* pSysMem);

public:
    static void BindVS(ConstantBuffer* constantBuffer, Graphics* graphics, UINT slot);
    static void BindPS(ConstantBuffer* constantBuffer, Graphics* graphics, UINT slot);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
    UINT sizeBytes;

private:
    NODEC_DISABLE_COPY(ConstantBuffer);
};

