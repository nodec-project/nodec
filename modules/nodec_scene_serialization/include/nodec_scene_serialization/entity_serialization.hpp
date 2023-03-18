#ifndef NODEC_SCENE_SERIALIZATION__ENTITY_SERIALIZATION_HPP_
#define NODEC_SCENE_SERIALIZATION__ENTITY_SERIALIZATION_HPP_

#include "serializable_component.hpp"
#include "serializable_entity.hpp"

#include <nodec_scene/scene_registry.hpp>

#include <unordered_map>
#include <vector>

namespace nodec_scene_serialization {

class EntitySerialization {
public:
    using SceneRegistry = nodec_scene::SceneRegistry;
    using SceneEntity = nodec_scene::SceneEntity;

private:
    class BaseComponentSerialization {
    public:
        virtual std::unique_ptr<BaseSerializableComponent> serialize(const void *component) const = 0;

        virtual void emplace_component(const BaseSerializableComponent *, SceneEntity, SceneRegistry &) const = 0;
    };

    template<typename Component, typename SerializableComponent>
    class ComponentSerialization : public BaseComponentSerialization {
        using Serializer = std::function<std::unique_ptr<SerializableComponent>(const Component &)>;
        using Emplacer = std::function<void(const SerializableComponent &, SceneEntity, SceneRegistry &)>;

    public:
        std::unique_ptr<BaseSerializableComponent> serialize(const void *component) const override {
            assert(component);

            return serializer(*static_cast<const Component *>(component));
        }

        void emplace_component(const BaseSerializableComponent *serializable_component, SceneEntity entity, SceneRegistry &scene_registry) const override {
            using namespace nodec;

            assert(serializable_component);
            assert(serializable_component->type_info() == type_id<SerializableComponent>());

            emplacer(*static_cast<const SerializableComponent *>(serializable_component), entity, scene_registry);
        }

    public:
        Serializer serializer;
        Emplacer emplacer;
    };

public:
    /**
     * @param serializer
     *   @code{.cpp}
     *   std::unique_ptr<SerializableComponent>(const Component&);
     *   @endcode
     *
     * @param emplacer
     *   @code{.cpp}
     *   void(const SerializableComponent&, SceneEntity, SceneRegistry&);
     *   @endcode
     */
    template<typename Component, typename SerializableComponent, typename Serializer, typename Emplacer>
    void register_component(Serializer &&serializer, Emplacer &&emplacer) {
        using namespace nodec;

        type_seq_index_type component_type_id = type_seq_index<Component>::value();
        type_seq_index_type serializable_component_type_id = type_seq_index<SerializableComponent>::value();

        assert(component_dict.find(component_type_id) == component_dict.end());
        assert(serializable_component_dict.find(serializable_component_type_id) == serializable_component_dict.end());

        auto serialization = std::make_shared<ComponentSerialization<Component, SerializableComponent>>();

        serialization->serializer = serializer;
        serialization->emplacer = emplacer;

        component_dict[component_type_id] = serialization;
        serializable_component_dict[serializable_component_type_id] = serialization;
    }

    std::unique_ptr<SerializableEntity> make_serializable_entity(
        const nodec_scene::SceneEntity &entity,
        const nodec_scene::SceneRegistry &scene_registry) const {
        using namespace nodec;

        auto serializable_entity = std::make_unique<SerializableEntity>();

        scene_registry.visit(entity, [=, &serializable_entity](const nodec::type_info type_info, void *comp) {
            assert(comp);

            auto iter = component_dict.find(type_info.seq_index());
            if (iter == component_dict.end()) return;

            auto &serialization = iter->second;
            assert(static_cast<bool>(serialization));

            auto serializable_component = serialization->serialize(comp);
            serializable_entity->components.push_back(std::move(serializable_component));
        });

        return serializable_entity;
    }

    /**
     * @brief Make the entity from the serializable entity.
     */
    nodec_scene::SceneEntity make_entity(
        const SerializableEntity *serializable_entity,
        nodec_scene::SceneRegistry &scene_registry) const {
        using namespace nodec;

        if (!serializable_entity) return entities::null_entity;

        auto entity = scene_registry.create_entity();

        for (auto &comp : serializable_entity->components) {
            if (!comp) continue;

            auto iter = serializable_component_dict.find(comp->type_info().seq_index());
            if (iter == serializable_component_dict.end()) continue;

            auto &serialization = iter->second;
            assert(static_cast<bool>(serialization));

            serialization->emplace_component(comp.get(), entity, scene_registry);
        }

        return entity;
    }

private:
    std::unordered_map<nodec::type_seq_index_type, std::shared_ptr<BaseComponentSerialization>> component_dict;
    std::unordered_map<nodec::type_seq_index_type, std::shared_ptr<BaseComponentSerialization>> serializable_component_dict;
};

} // namespace nodec_scene_serialization

#endif
