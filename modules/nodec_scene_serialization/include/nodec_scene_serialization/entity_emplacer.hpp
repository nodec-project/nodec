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

    nodec_scene::SceneEntity emplace(SerializableEntity *source, const nodec_scene::SceneEntity &parent, nodec_scene::Scene &scene) const {
        using namespace nodec::entities;
        using namespace nodec_scene;

        if (source == nullptr) return null_entity;

        struct Context {
            SerializableEntity *entity;
            SceneEntity parent;
        };

        SceneEntity root{null_entity};

        std::stack<Context> stack;
        stack.push({source, parent});

        while (!stack.empty()) {
            auto context = std::move(stack.top());
            stack.pop();

            auto entity = serialization_.make_entity(context.entity, scene.registry());
            assert(entity != null_entity);

            if (root == null_entity) root = entity;

            if (context.parent == null_entity) {
                // if no parent, just attach the Hierarchy component as root entity.
                scene.registry().emplace_component<nodec_scene::components::Hierarchy>(entity);
            } else {
                scene.hierarchy_system().append_child(context.parent, entity);
            }

            for (auto &child : context.entity->children) {
                stack.push({child.get(), entity});
            }
        }

        return root;
    }

private:
    const SceneSerialization &serialization_;
};
} // namespace nodec_scene_serialization

#endif