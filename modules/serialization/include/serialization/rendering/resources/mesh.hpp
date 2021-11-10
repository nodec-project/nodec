#ifndef SERIALIZATION__RENDERING__RESOURCES__MESH_HPP_
#define SERIALIZATION__RENDERING__RESOURCES__MESH_HPP_

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#include <serialization/nodec/vector3.hpp>
#include <serialization/nodec/vector2.hpp>

#include <rendering/resources/mesh.hpp>


namespace rendering {
namespace resources {


template<class Archive>
void serialize(Archive& archive, Mesh::Vertex& vert) {
    archive(
        cereal::make_nvp("position", vert.position),
        cereal::make_nvp("normal", vert.normal),
        cereal::make_nvp("uv", vert.uv),
        cereal::make_nvp("tangent", vert.tangent)
    );
}


template<class Archive>
void serialize(Archive& archive, Mesh& mesh) {
    archive(
        cereal::make_nvp("vertices", mesh.vertices),
        cereal::make_nvp("triangles", mesh.triangles)
    );
}


}
}

#endif