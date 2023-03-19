#ifndef NODEC_SCENE_SERIALIZATION__ENTITY_EMPLACER_HPP_
#define NODEC_SCENE_SERIALIZATION__ENTITY_EMPLACER_HPP_

#include "entity_serialization.hpp"
#include "serializable_entity.hpp"

#include <nodec_scene/scene.hpp>

namespace nodec_scene_serialization {

class EntityEmplacer final {
public:
    EntityEmplacer(const nodec_scene::SceneEntity &entity,
                       const nodec_scene::SceneRegistry &registry,
                       const SceneSerialization &serialization)
        : serialization_(serialization) {
    }


    nodec_scene::SceneEntity emplace(const SerializableEntity *entity, nodec_scene::Scene &scene) const {
    }

private:
    const SceneSerialization &serialization_;
};
} // namespace nodec_scene_serialization

#endif