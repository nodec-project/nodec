#ifndef NODEC_SCENE_SERIALIZATION__SERIALIZABLE_ENTITY_HPP_
#define NODEC_SCENE_SERIALIZATION__SERIALIZABLE_ENTITY_HPP_

#include "serializable_component.hpp"

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>

#include <vector>

namespace nodec_scene_serialization {

/**
 * @brief The SerializableEntity class is used for serialization of entity.
 *
 * The SerializableEntity has the serializable components and serializable child entities.
 * It can be serialized using cereal library.
 */
class SerializableEntity {
public:
    std::vector<std::unique_ptr<BaseSerializableComponent>> components;
    std::vector<std::unique_ptr<SerializableEntity>> children;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("components", components));
        archive(cereal::make_nvp("children", children));
    }
};

} // namespace nodec_scene_serialization

#endif