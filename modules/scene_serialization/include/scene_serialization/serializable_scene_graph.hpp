#ifndef SCENE_SERIALIZATION__SERIALIZABLE_SCENE_GRAPH_HPP_
#define SCENE_SERIALIZATION__SERIALIZABLE_SCENE_GRAPH_HPP_

#include "serializable_entity_node.hpp"

#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include <memory>
#include <vector>


namespace scene_serialization {

class SerializableSceneGraph {
public:
    std::vector<std::shared_ptr<SerializableEntityNode>> roots;
};

template<class Archive>
void serialize(Archive& archive, SerializableSceneGraph& graph) {

    archive(cereal::make_nvp("roots", graph.roots));

}

}

#endif