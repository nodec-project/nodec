#ifndef NODEC_SCENE_SERIALIZATION__SCENE_SERIALIZATION_HPP_
#define NODEC_SCENE_SERIALIZATION__SCENE_SERIALIZATION_HPP_

#include "serializable_component.hpp"
#include "serializable_entity_node.hpp"

#include <nodec/logging.hpp>
#include <nodec_scene/scene_registry.hpp>

#include <unordered_map>
#include <vector>

namespace nodec_scene_serialization {

class SceneSerialization {
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
     * @param preloader
     *  @code{.cpp}
     *
     *  @endcode
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

    std::unique_ptr<SerializableEntityNode> make_serializable_node(
        const SceneEntity entity,
        const SceneRegistry &scene_registry) const {
        using namespace nodec;

        auto node = std::make_unique<SerializableEntityNode>();

        auto exception_handler = [=](type_seq_index_type component_type_id) {
            std::string details;

            try {
                throw;
            } catch (std::exception &e) {
                details = e.what();
            } catch (...) {
                details = "Unknown";
            }

            logging::WarnStream(__FILE__, __LINE__)
                << "Failed to serialize component ( type_seq_id: " << component_type_id
                << " ) of entity ( entity: 0x" << std::hex << entity << " )\n"
                << "details: \n"
                << details;
        };

        scene_registry.visit(entity, [=, &node](const nodec::type_info type_info, void *comp) {
            assert(comp);

            try {
                auto &serialization = component_dict.at(type_info.seq_index());

                assert(static_cast<bool>(serialization));

                auto serializable_component = serialization->serialize(comp);
                node->components.push_back(std::move(serializable_component));
            } catch (...) {
                exception_handler(type_info.seq_index());
            }
        });

        return node;
    }

    SceneEntity make_entity(
        const SerializableEntityNode* entity_node,
        SceneRegistry &scene_registry) const {
        using namespace nodec;

        if (!entity_node) {
            return entities::null_entity;
        }

        auto exception_handler = [](type_seq_index_type component_type_id) {
            std::string details;

            try {
                throw;
            } catch (std::exception &e) {
                details = e.what();
            } catch (...) {
                details = "Unknown";
            }

            logging::WarnStream(__FILE__, __LINE__)
                << "Failed to emplace component from serializable component ( type_seq_id: " << component_type_id << " )\n"
                << "details: \n"
                << details;
        };

        auto entity = scene_registry.create_entity();

        for (auto &comp : entity_node->components) {
            if (!comp) continue;

            try {
                auto &serialization = serializable_component_dict.at(comp->type_info().seq_index());

                assert(static_cast<bool>(serialization));

                serialization->emplace_component(comp.get(), entity, scene_registry);
            } catch (...) {
                exception_handler(comp->type_info().seq_index());
            }
        }

        return entity;
    }

private:
    std::unordered_map<nodec::type_seq_index_type, std::shared_ptr<BaseComponentSerialization>> component_dict;
    std::unordered_map<nodec::type_seq_index_type, std::shared_ptr<BaseComponentSerialization>> serializable_component_dict;
};

} // namespace nodec_scene_serialization

#endif
