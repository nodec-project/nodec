#pragma once

#include <rendering/resources/mesh.hpp>

#include <Graphics/VertexBuffer.hpp>
#include <Graphics/IndexBuffer.hpp>

#include <nodec/vector3.hpp>
#include <nodec/vector2.hpp>


class Mesh : public rendering::resources::Mesh {

public:
    struct Vertex {
        nodec::Vector3f position;
        nodec::Vector3f normal;
        nodec::Vector2f uv;
        nodec::Vector3f tangent;

        Vertex(const nodec::Vector3f& position) :
            position(position) {
        };

        Vertex(const nodec::Vector3f& position, const nodec::Vector3f& normal) :
            position(position),
            normal(normal) {
        };

        Vertex(const nodec::Vector3f& position, const nodec::Vector3f& normal, const nodec::Vector2f& uv) :
            position(position),
            normal(normal),
            uv(uv) {
        };

    };

    std::vector<Vertex> vertices;

    std::vector<uint16_t> triangles;

    void updateDeviceMemory() {

    }


private:
    std::unique_ptr<VertexBuffer> vertex_buffer_;
    std::unique_ptr<IndexBuffer> index_buffer_;
};