#pragma once

#include <Graphics/IndexBuffer.hpp>
#include <Graphics/VertexBuffer.hpp>

#include <nodec_rendering/resources/mesh.hpp>

#include <nodec/vector2.hpp>
#include <nodec/vector3.hpp>

#include <cassert>

class MeshBackend : public nodec_rendering::resources::Mesh {
public:
    struct Vertex {
        nodec::Vector3f position;
        nodec::Vector3f normal;
        nodec::Vector2f uv;
        nodec::Vector3f tangent;
    };

    std::vector<Vertex> vertices;
    std::vector<uint16_t> triangles;

    void update_device_memory(Graphics *graphics) {
        vertex_buffer_.reset();
        index_buffer_.reset();

        vertex_buffer_.reset(
            new VertexBuffer(
                graphics,
                vertices.size() * sizeof(Vertex),
                sizeof(Vertex),
                vertices.data()));

        index_buffer_.reset(
            new IndexBuffer(
                graphics,
                triangles.size(),
                triangles.data()));
    }

    VertexBuffer *vertex_buffer() {
        return vertex_buffer_.get();
    }

    IndexBuffer *index_buffer() {
        return index_buffer_.get();
    }

    void bind(Graphics *gfx) {
        if (vertex_buffer_) {
            vertex_buffer_->Bind(gfx);
        }

        if (index_buffer_) {
            index_buffer_->Bind(gfx);
        }
    }

private:
    std::unique_ptr<VertexBuffer> vertex_buffer_;
    std::unique_ptr<IndexBuffer> index_buffer_;
};