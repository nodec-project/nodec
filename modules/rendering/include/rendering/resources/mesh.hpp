#ifndef RENDERING__RESOURCES__MESH_HPP_
#define RENDERING__RESOURCES__MESH_HPP_

#include <nodec/vector3.hpp>
#include <nodec/vector2.hpp>

namespace rendering {
namespace resources {


class Mesh {

public:
    struct Vertex {
        nodec::Vector3f position;
        nodec::Vector3f normal;
        nodec::Vector2f uv;
        nodec::Vector3f tangent;

        Vertex() = default;

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

    bool is_dirty{ false };
};

}
}


#endif
