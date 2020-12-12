#pragma once

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

#include <nodec/nodec_object.hpp>

#include <unordered_map>

#include <d3d11.h>
#include <wrl.h>

class GraphicsResources
{
public:
    std::unordered_map<nodec::NodecObject::ID, std::shared_ptr<VertexBuffer>> vertexBufferMap;
    std::unordered_map<nodec::NodecObject::ID, std::shared_ptr<IndexBuffer>> indexBufferMap;

public:
    GraphicsResources();

private:
    NODEC_DISABLE_COPY(GraphicsResources);
};