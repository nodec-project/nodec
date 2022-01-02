#ifndef SERIALIZATION__RENDERING__COMPONENTS__MESH_RENDERER_HPP_
#define SERIALIZATION__RENDERING__COMPONENTS__MESH_RENDERER_HPP_

#include <scene_serialization/scene_serialization.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>


namespace rendering {
namespace components {

class SerializableMeshRenderer : public scene_serialization::BaseSerializableComponent {

public:
    SerializableMeshRenderer()
        : BaseSerializableComponent(this) {
    }

    std::vector<std::string> meshes;
    std::vector<std::string> materials;


    template<class Archive>
    void serialize(Archive& archive) {

        archive(cereal::make_nvp("meshes", meshes));
        archive(cereal::make_nvp("materials", materials));

    }
};


}
}

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(rendering::components::SerializableMeshRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(scene_serialization::BaseSerializableComponent, rendering::components::SerializableMeshRenderer);


#endif