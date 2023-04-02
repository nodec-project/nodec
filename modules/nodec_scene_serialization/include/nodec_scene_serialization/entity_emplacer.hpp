#ifndef NODEC_SCENE_SERIALIZATION__ENTITY_EMPLACER_HPP_
#define NODEC_SCENE_SERIALIZATION__ENTITY_EMPLACER_HPP_

#include "scene_serialization.hpp"
#include "serializable_entity.hpp"

#include <nodec_scene/components/hierarchy.hpp>
#include <nodec_scene/scene.hpp>

#include <cassert>
#include <stack>

namespace nodec_scene_serialization {

/**
 * @brief The EntityEmplacer class allows emplacing the nodes of graph to the scene as new scene entity.
 */
class EntityEmplacer {
public:
    EntityEmplacer(const SceneSerialization &serialization)
        : serialization_{serialization} {}

    void emplace(SerializableEntity *source, const nodec_scene::SceneEntity &target, nodec_scene::Scene &scene) const {
        using namespace nodec::entities;
        using namespace nodec_scene;

        assert(scene.registry().is_valid(target));

        if (source == nullptr) return;

        serialization_.emplace_components(source, target, scene.registry());

        struct Context {
            SerializableEntity *entity;
            SceneEntity parent;
        };

        std::stack<Context> stack;
        for (auto &child : source->children) {
            stack.push({child.get(), target});
        }

        while (!stack.empty()) {
            auto context = std::move(stack.top());
            stack.pop();

            auto entity = serialization_.make_entity(context.entity, scene.registry());
            assert(entity != null_entity);

            // The context.parent must exists.
            scene.hierarchy_system().append_child(context.parent, entity);

            for (auto &child : context.entity->children) {
                stack.push({child.get(), entity});
            }
        }
    }

private:
    const SceneSerialization &serialization_;
};
} // namespace nodec_scene_serialization

#endif