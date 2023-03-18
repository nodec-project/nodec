#ifndef NODEC_SCENE_SERIALIZATION__SCENE_SERIALIZATION_HPP_
#define NODEC_SCENE_SERIALIZATION__SCENE_SERIALIZATION_HPP_

#include "scene_serialization.hpp"
#include "serializable_entity.hpp"

#include <nodec_scene/scene.hpp>

namespace nodec_scene_serialization {

class SceneSerialization final {
public:
    SceneSerialization(const nodec_scene::SceneEntity &entity,
                       const nodec_scene::SceneRegistry &registry,
                       const EntitySerialization &serialization)
        : serialization_(serialization) {
    }

    std::shared_ptr<SerializableEntity> serialize() {
        auto root = serialization_.make_serializable_entity(entity, registry);
    }

private:
    const EntitySerialization &serialization_;
};
} // namespace nodec_scene_serialization

#endif