#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__RESOURCES__MESH_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__RESOURCES__MESH_HPP_

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#include <nodec_serialization/nodec/vector2.hpp>
#include <nodec_serialization/nodec/vector3.hpp>

#include <nodec_rendering/resources/mesh.hpp>

#include <vector>

namespace nodec_rendering {
namespace resources {

struct SerializableMesh {
    struct Vertex {
        nodec::Vector3f position;
        nodec::Vector3f normal;
        nodec::Vector2f uv;
        nodec::Vector3f tangent;
    };

    std::vector<Vertex> vertices;

    std::vector<uint16_t> triangles;
};

template<class Archive>
void serialize(Archive &archive, SerializableMesh::Vertex &vert) {
    archive(
        cereal::make_nvp("position", vert.position),
        cereal::make_nvp("normal", vert.normal),
        cereal::make_nvp("uv", vert.uv),
        cereal::make_nvp("tangent", vert.tangent));
}
template<class Archive>
void serialize(Archive &archive, SerializableMesh &mesh) {
    archive(
        cereal::make_nvp("vertices", mesh.vertices),
        cereal::make_nvp("triangles", mesh.triangles));
}

} // namespace resources
} // namespace nodec_rendering

#endif