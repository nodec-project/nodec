#ifndef SCENE_SERIALIZATION_SCENE_SERIALIZATION_HPP_
#define SCENE_SERIALIZATION_SCENE_SERIALIZATION_HPP_

#include <scene_set/scene_registry.hpp>

#include <vector>

#include <cereal/types/polymorphic.hpp>


//CEREAL_REGISTER_POLYMORPHIC_RELATION()

namespace scene_serialization {

class BaseSerializableComponent {

public:
    template<class Derived>
    BaseSerializableComponent()
        : type_id_{ nodec::type_seq<Derived>::value() } {

    }

public:
    nodec::TypeId type_id() const { return type_id_; }

private:
    const nodec::TypeId type_id_;

    //using SceneEntity = scene_set::SceneEntity;
    //using SceneRegistry = scene_set::SceneRegistry;
//
//public:
//    virtual void emplace_component(SceneRegistry& registry, const SceneEntity entity) const = 0;

};

//
//template<typename Component>
//class SerializableComponent : public BaseSerializableComponent {
//    
//    using SceneEntity = scene_set::SceneEntity;
//    using SceneRegistry = scene_set::SceneRegistry;
//
//public:
//    void emplace_component(SceneRegistry& registry, const SceneEntity entity) const override {
//        registry.emplace_component<Component>(entity);
//        registry.get_component<Component>(entity) = data;
//    }
//
//public:
//    Component data;
//
//};

//template<class Archive, typename Component>
//void serialize(Archive& archive, SerializableComponent<Component>& serializer) {
//
//    archive(serializer.component);
//
//}

class SerializableEntityNode {

    using SceneEntity = scene_set::SceneEntity;
    using SceneRegistry = scene_set::SceneRegistry;

//public:
//    SceneEntity emplace_entity(SceneRegistry& registry) {
//        auto entity = registry.create_entity();
//
//        for (auto& comp : components) {
//            if (!comp) {
//                continue;
//            }
//            comp->emplace_component(registry, entity);
//        }
//        return entity;
//    }

public:
    std::vector<std::shared_ptr<SerializableEntityNode>> children;
    std::vector<std::shared_ptr<BaseSerializableComponent>> components;

};

class SerializableSceneGraph {

    std::vector<SerializableEntityNode> roots;
};


namespace internal {

}


class SceneSerialization {
    using SceneRegistry = scene_set::SceneRegistry;
    using SceneEntity = scene_set::SceneEntity;

    class BaseComponentSerialization {
    public:
        virtual std::shared_ptr<BaseSerializableComponent> serialize(const void* component) const = 0;

        virtual void emplace_component(const BaseSerializableComponent*, SceneEntity, SceneRegistry&) const = 0;

    };

    template<typename Component, typename SerializableComponent>
    class ComponentSerialization : public BaseComponentSerialization {
        using Serializer = std::function<std::shared_ptr<SerializableComponent>(const Component&)>;
        using Emplacer = std::function<void(const SerializableComponent&, SceneEntity, SceneRegistry&)>;

        //using Deserializer = std::function<std::shared_ptr<Component>(const SerializableComponent&)>;

    public:
        std::shared_ptr<BaseSerializableComponent> serialize(const void* component) const override {
            assert(component);

            return serializer(*static_cast<Component>(component));
        }

        void emplace_component(const BaseSerializableComponent* serializable_component, SceneEntity entity, SceneRegistry& scene_registry) const override {
            using namespace nodec;

            assert(serializable_component);
            assert(serializable_component->type_id() == type_seq<SerializableComponent>::value());

            emplacer(*static_cast<SerializableComponent*>(serializable_component), entity, scene_registry);
        }

    public:
        Serializer serializer;
        Emplacer emplacer;
        //Deserializer deserializer;
    };

public:

    template<typename Component, typename SerializableComponent, typename Serializer, typename Emplacer>
    void register_component(Serializer&& serializer, Emplacer&& emplacer) {

        using namespace nodec;

        TypeId component_type_id = type_seq<Component>::value();
        TypeId serializable_component_type_id = type_seq<SerializableComponent>::value();

        assert(component_dict.find(component_type_id) != component_dict.end());
        assert(serializable_component_dict.find(serializable_component_type_id) != serializable_component_dict.end());

        auto serialization = std::make_shared<ComponentSerialization<Component, SerializableComponent>>();

        serialization->serializer = serializer;
        serialization->emplacer = emplacer;

        component_dict[component_type_id] = serialization;
        serializable_component_dict[serializable_component_type_id] = serialization;

    }

    //template<typename SerializableComponent, typename Component>
    //SerializableComponent serialize(const Component& component) const {

    //    using namespace nodec;

    //    auto pair = std::make_pair(type_seq<Component>::value(), type_seq<SerializableComponent>::value());
    //    auto* serialization = static_cast<ComponentSerialization<Component, SerializableComponent>*>(component_serializations.at(pair).get());

    //    return serialization->serializer(component);
    //}

    //template<typename Component, typename SerializableComponent>
    //Component deserialize(const SerializableComponent& serializable_component) const {

    //    using namespace nodec;



    //}

    std::shared_ptr<SerializableEntityNode> make_serializable_node(const SceneEntity entity, const SceneRegistry& scene_registry) {
        using namespace nodec;

        auto node = std::make_shared<SerializableEntityNode>();

        auto exception_handler = [=](TypeId component_type_id) {

            std::string details;

            try {
                throw;
            }
            catch (std::exception& e) {
                details = e.what();
            }
            catch (...) {
                details = "Unknown";
            }

            logging::WarnStream(__FILE__, __LINE__)
                << "Failed to serialize component ( type_seq_id: " << component_type_id 
                << " ) of entity ( entity: 0x" << std::hex << entity << " )\n"
                << "details: \n"
                << details;
        };

        scene_registry.visit(entity, [=, &node](int type_id, void* comp) {
            assert(comp);

            try {
                auto& serialization = component_dict.at(type_id);

                assert(static_cast<bool>(serialization));
                
                auto serializable_component = serialization->serialize(comp);
                node->components.push_back(serializable_component);
            }
            catch (...) {
                exception_handler(type_id);
            }

            });

        return node;
    }

    SceneEntity emplace_entity(std::shared_ptr<SerializableEntityNode> entity_node, SceneRegistry& scene_registry) {
        using namespace nodec;

        if (!entity_node) {
            return entities::null_entity;
        }


        auto exception_handler = [](TypeId component_type_id) {

            std::string details;

            try {
                throw;
            }
            catch (std::exception& e) {
                details = e.what();
            }
            catch (...) {
                details = "Unknown";
            }

            logging::WarnStream(__FILE__, __LINE__)
                << "Failed to emplace component from serializable component ( type_seq_id: " << component_type_id << " )\n"
                << "details: \n"
                << details;
        };

        auto entity = scene_registry.create_entity();

        for (auto& comp : entity_node->components) {
            if (!comp) { continue; }

            try {
                auto& serialization = serializable_component_dict.at(comp->type_id());

                assert(static_cast<bool>(serialization));

                serialization->emplace_component(comp.get(), entity, scene_registry);
            }
            catch (...) {
                exception_handler(comp->type_id());
            }
        }

        return entity;
    }


private:
    std::unordered_map<nodec::TypeId, std::shared_ptr<BaseComponentSerialization>> component_dict;
    std::unordered_map<nodec::TypeId, std::shared_ptr<BaseComponentSerialization>> serializable_component_dict;
};

}



#endif
