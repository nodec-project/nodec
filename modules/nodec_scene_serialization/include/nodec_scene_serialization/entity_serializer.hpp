#ifndef NODEC_SCENE_SERIALIZATION__ENTITY_SERIALIZER_HPP_
#define NODEC_SCENE_SERIALIZATION__ENTITY_SERIALIZER_HPP_

#include "scene_serialization.hpp"

namespace nodec_scene_serialization {

class EntitySerializer final {
public:
    EntitySerializer(const nodec_scene::SceneEntity &entity, const nodec_scene::Scene &scene, const SceneSerialization &serialization)
        : serialization_(serialization) {}

    std::shared_ptr<SerializableEntity> serialize(const nodec_scene::SceneEntity &entity, const nodec_scene::Scene &scene) const {
        auto root = serialization_.make_serializable_entity(entity, scene.registry());

        // TODO: Push the root entity.
        // TODO: while the stack is not empty.
        // TODO:    Pop the entity from stack.
        // TODO:    Get hierarchy component.
        // TODO:    Get child entities.
        // TODO:    Foreach all children.
        // TODO:        Serialize child.
        // TODO:        Append child to current entity.
        // TODO:        Push child to the stack.
        // TODO: Return root entity.
    }

private:
    const SceneSerialization &serialization_;
    const nodec_scene::Scene &scene_;
    const nodec_scene::SceneEntity target_entity_;
};
} // namespace nodec_scene_serialization

#endif
