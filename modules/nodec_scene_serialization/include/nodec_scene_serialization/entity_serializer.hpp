#ifndef NODEC_SCENE_SERIALIZATION__ENTITY_SERIALIZER_HPP_
#define NODEC_SCENE_SERIALIZATION__ENTITY_SERIALIZER_HPP_

#include "scene_serialization.hpp"
#include "serializable_entity.hpp"

#include <nodec_scene/scene.hpp>

namespace nodec_scene_serialization {

class EntitySerializer final {
public:
    EntitySerializer() {
    }

    std::shared_ptr<SerializableEntity> serialize(nodec_scene::SceneEntity entity,
                                                  const nodec_scene::SceneRegistry &registry,
                                                  const SceneSerialization &serialization) {
        

    }
};
} // namespace nodec_scene_serialization

#endif