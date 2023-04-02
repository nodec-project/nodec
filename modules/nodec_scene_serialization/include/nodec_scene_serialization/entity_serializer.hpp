#ifndef NODEC_SCENE_SERIALIZATION__ENTITY_SERIALIZER_HPP_
#define NODEC_SCENE_SERIALIZATION__ENTITY_SERIALIZER_HPP_

#include "components/prefab.hpp"
#include "scene_serialization.hpp"
#include "serializable_entity.hpp"

#include <nodec_scene/components/hierarchy.hpp>
#include <nodec_scene/scene.hpp>

#include <cassert>
#include <stack>

namespace nodec_scene_serialization {

class EntitySerializer final {
public:
    EntitySerializer(const SceneSerialization &serialization)
        : serialization_(serialization) {}

    /**
     * @brief
     *
     * @todo Add filter.
     *
     * @param target_entity
     * @param scene
     * @return std::unique_ptr<SerializableEntity>
     */
    template<typename Filter>
    std::unique_ptr<SerializableEntity> serialize(const nodec_scene::SceneEntity &target_entity, const nodec_scene::Scene &scene, Filter &&filter) const {
        using namespace nodec::entities;
        using namespace nodec_scene;
        using namespace nodec_scene::components;
        using namespace components;

        // If the entity is null, the process does not continue.
        // This is to guarantee that the null entity will not be included in the context that follows.
        if (target_entity == null_entity) return {};

        struct Context {
            SceneEntity entity;
            SerializableEntity *parent;
        };

        const auto &scene_registry = scene.registry();
        std::unique_ptr<SerializableEntity> root;

        std::stack<Context> stack;
        stack.push({target_entity, nullptr});

        while (!stack.empty()) {
            auto context = std::move(stack.top());
            stack.pop();

            // If we don't want to serialize the current entity, add a filter function here.

            SerializableEntity *ser_entity_ptr;
            {
                auto ser_entity = serialization_.make_serializable_entity(context.entity, scene_registry);
                ser_entity_ptr = ser_entity.get();
                if (!root) {
                    // since this entity is root, it should have no parent.
                    assert(context.parent == nullptr);
                    root = std::move(ser_entity);
                } else {
                    assert(context.parent != nullptr);
                    context.parent->children.emplace_back(std::move(ser_entity));
                }
            }

            // A filter function for when we want to serialize the components of current entity
            // but not its children.
            // Mainly used for prefab system.
            if (!filter(context.entity)) continue;

            auto *hierarchy = scene_registry.try_get_component<Hierarchy>(context.entity);
            if (hierarchy == nullptr) continue;

            auto child = hierarchy->first;
            ser_entity_ptr->children.reserve(hierarchy->child_count);
            while (child != null_entity) {
                stack.push({child, ser_entity_ptr});
                // The hierarchy should be contained since the parent has the hierarchy.
                child = scene_registry.get_component<Hierarchy>(child).next;
            }
        }

        return root;
    }

    std::unique_ptr<SerializableEntity> serialize(const nodec_scene::SceneEntity &target_entity, const nodec_scene::Scene &scene) const {
        return serialize(target_entity, scene, [](const nodec_scene::SceneEntity &) { return true; });
    }

private:
    const SceneSerialization &serialization_;
};
} // namespace nodec_scene_serialization

#endif
