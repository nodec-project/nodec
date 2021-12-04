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

        assert(component_serializations.find({ component_type_id, serializable_component_type_id }) != component_serializations.end());

        assert(component_dict.find(component_type_id) != component_dict.end());
        assert(serializable_component_dict.find(serializable_component_type_id) != serializable_component_dict.end());

        auto pair = std::make_pair(component_type_id, serializable_component_type_id);
        component_dict[component_type_id] = pair;
        serializable_component_dict[serializable_component_type_id] = pair;

        component_serializations[pair].reset(new ComponentSerialization<Component, SerializableComponent>());
        auto* serialization = static_cast<ComponentSerialization<Component, SerializableComponent>*>(component_serializations[pair].get());
        serialization->serializer = serializer;
        serialization->emplacer = emplacer;


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

        auto exception_handler = []() {

        };

        scene_registry.visit(entity, [=, &node](int type_id, void* comp) {

            try {
                auto& pair = component_dict.at(type_id);
                auto& serialization = component_serializations.at(pair);

                assert(serialization);
                assert(comp);

                auto serializable_component = serialization->serialize(comp);
                node->components.push_back(serializable_component);
            }
            catch (...) {
                exception_handler();
            }

            });

        return node;
    }

    SceneEntity emplace_entity(std::shared_ptr<SerializableEntityNode> entity_node, SceneRegistry& scene_registry) {
        using namespace nodec;

        if (!entity_node) {
            return entities::null_entity;
        }


        auto exception_handler = []() {

        };

        auto entity = scene_registry.create_entity();

        for (auto& comp : entity_node->components) {
            if (!comp) { continue; }

            try {
                auto& pair = serializable_component_dict.at(comp->type_id());
                auto& serialization = component_serializations.at(pair);

                assert(serialization);

                serialization->emplace_component(comp.get(), entity, scene_registry);
            }
            catch (...) {
                exception_handler();
            }

            
        }

        return entity;
    }

private:


private:
    std::map<std::pair<nodec::TypeId, nodec::TypeId>, std::unique_ptr<BaseComponentSerialization>> component_serializations;
    std::unordered_map<nodec::TypeId, std::pair<nodec::TypeId, nodec::TypeId>> component_dict;
    std::unordered_map<nodec::TypeId, std::pair<nodec::TypeId, nodec::TypeId>> serializable_component_dict;


};

}



#endif
