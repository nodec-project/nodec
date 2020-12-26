#pragma once

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexShader.hpp"
#include "PixelShader.hpp"
#include "ConstantBuffer.hpp"
#include "Texture.hpp"

#include <nodec/nodec_object.hpp>

#include <unordered_map>

#include <d3d11.h>
#include <wrl.h>

class GraphicsResources
{
public:
    std::unordered_map<nodec::NodecObject::ID, std::shared_ptr<VertexBuffer>> vertexBufferMap;
    std::unordered_map<nodec::NodecObject::ID, std::shared_ptr<IndexBuffer>> indexBufferMap;
    
    std::unordered_map<nodec::NodecObject::ID, std::shared_ptr<VertexShader>> vertexShaderMap;
    std::unordered_map<nodec::NodecObject::ID, std::shared_ptr<PixelShader>> pixelShaderMap;

    std::unordered_map<nodec::NodecObject::ID, std::shared_ptr<ConstantBuffer>> constantBufferMap;

    std::unordered_map<nodec::NodecObject::ID, std::shared_ptr<Texture>> textureMap;

public:
    GraphicsResources();

private:
    NODEC_DISABLE_COPY(GraphicsResources);
};