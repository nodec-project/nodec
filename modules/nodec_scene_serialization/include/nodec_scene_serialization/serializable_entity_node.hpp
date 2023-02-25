#ifndef NODEC_SCENE_SERIALIZATION__SERIALIZABLE_ENTITY_NODE_HPP_
#define NODEC_SCENE_SERIALIZATION__SERIALIZABLE_ENTITY_NODE_HPP_

#include "serializable_component.hpp"

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>

#include <vector>

namespace nodec_scene_serialization {

class SerializableEntityNode {
public:
    std::vector<std::unique_ptr<SerializableEntityNode>> children;
    std::vector<std::unique_ptr<BaseSerializableComponent>> components;
};

template<class Archive>
void serialize(Archive &archive, SerializableEntityNode &node) {
    archive(cereal::make_nvp("components", node.components));
    archive(cereal::make_nvp("children", node.children));
}

} // namespace nodec_scene_serialization

#endif