#pragma once

#include <scene_serialization/scene_serialization.hpp>
#include <scene_set/components/basic.hpp>
#include <rendering/components/mesh_renderer.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>


class SerializableMeshRenderer : public scene_serialization::BaseSerializableComponent {
    using MeshRenderer = rendering::components::MeshRenderer;

public:
    SerializableMeshRenderer()
        : BaseSerializableComponent(this) {
    }

    std::vector<std::string> meshes;
    std::vector<std::string> materials;

};

template<class Archive>
void serialize(Archive& archive, SerializableMeshRenderer& renderer) {

    archive(cereal::make_nvp("meshes", renderer.meshes));
    archive(cereal::make_nvp("materials", renderer.materials));

}


#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(SerializableMeshRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(scene_serialization::BaseSerializableComponent, SerializableMeshRenderer);

