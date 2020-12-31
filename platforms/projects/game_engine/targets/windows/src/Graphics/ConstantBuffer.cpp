#include "Graphics/ConstantBuffer.hpp"

ConstantBuffer::ConstantBuffer(
    Graphics* graphics, UINT sizeBytes, const void* pSysMem) :
    sizeBytes(sizeBytes)
{
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeBytes;
    cbd.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA csd ={};
    csd.pSysMem = pSysMem;

    graphics->ThrowIfError(
        graphics->GetDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer),
        __FILE__,
        __LINE__
    );

}

void ConstantBuffer::Update(Graphics* graphics, const void* pSysMem)
{
    D3D11_MAPPED_SUBRESOURCE msr;
    graphics->ThrowIfError(
        graphics->GetContext()->Map(
            pConstantBuffer.Get(), 0u,
            D3D11_MAP_WRITE_DISCARD, 0u,
            &msr
        ),
        __FILE__, __LINE__
    );
    memcpy(msr.pData, pSysMem, sizeBytes);
    graphics->GetContext()->Unmap(pConstantBuffer.Get(), 0u);
}

void ConstantBuffer::BindVS(Graphics* graphics, UINT slot)
{
    graphics->GetInfoLogger()->SetLatest();
    graphics->GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}

void ConstantBuffer::BindPS(Graphics* graphics, UINT slot)
{
    graphics->GetInfoLogger()->SetLatest();
    graphics->GetContext()->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}