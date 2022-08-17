#ifndef SCENE_SERIALIZATION__SERIALIZABLE_ENTITY_NODE_HPP_
#define SCENE_SERIALIZATION__SERIALIZABLE_ENTITY_NODE_HPP_

#include "serializable_component.hpp"

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>

#include <vector>


namespace scene_serialization {

class SerializableEntityNode {

public:
    std::vector<std::shared_ptr<SerializableEntityNode>> children;
    std::vector<std::shared_ptr<BaseSerializableComponent>> components;

};


template<class Archive>
void serialize(Archive& archive, SerializableEntityNode& node) {

    archive(cereal::make_nvp("components", node.components));
    archive(cereal::make_nvp("children", node.children));

}

}


#endif