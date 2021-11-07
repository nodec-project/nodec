#pragma once

#include <rendering/resources/mesh.hpp>

#include <Graphics/VertexBuffer.hpp>
#include <Graphics/IndexBuffer.hpp>

#include <nodec/vector3.hpp>
#include <nodec/vector2.hpp>


class MeshBackend : public rendering::resources::Mesh {
    void updateDeviceMemory() {

    }


private:
    std::unique_ptr<VertexBuffer> vertex_buffer_;
    std::unique_ptr<IndexBuffer> index_buffer_;
};