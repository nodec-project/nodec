#ifndef SERIALIZATION__RENDERING__RESOURCES__MESH_HPP_
#define SERIALIZATION__RENDERING__RESOURCES__MESH_HPP_

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#include <serialization/nodec/vector3.hpp>
#include <serialization/nodec/vector2.hpp>

#include <rendering/resources/mesh.hpp>

#include <vector>


namespace rendering {
namespace resources {

struct SerializableMesh {
    struct Vertex {
        nodec::Vector3f position;
        nodec::Vector3f normal;
        nodec::Vector2f uv;
        nodec::Vector3f tangent;

        template<class Archive>
        void serialize(Archive& archive)   {
            archive(
                cereal::make_nvp("position", position),
                cereal::make_nvp("normal", normal),
                cereal::make_nvp("uv", uv),
                cereal::make_nvp("tangent", tangent)
                );
        }
    };

    std::vector<Vertex> vertices;

    std::vector<uint16_t> triangles;


    template<class Archive>
    void serialize(Archive& archive) {
        archive(
            cereal::make_nvp("vertices", vertices),
            cereal::make_nvp("triangles", triangles)
        );
    }
};


}
}

#endif