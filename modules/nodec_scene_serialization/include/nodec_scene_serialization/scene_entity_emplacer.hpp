#ifndef NODEC_SCENE_SERIALIZATION__ENTITIY_EMPLACER_HPP_
#define NODEC_SCENE_SERIALIZATION__ENTITIY_EMPLACER_HPP_

#include "scene_serialization.hpp"
#include "serializable_scene_graph.hpp"

#include <nodec_scene/scene.hpp>

#include <cassert>

namespace nodec_scene_serialization {

/**
 * @brief The SceneEntityEmplacer class allows emplacing the nodes of graph to the scene as new scene entity.
 *
 */
class SceneEntityEmplacer {
public:
    SceneEntityEmplacer(const SerializableSceneGraph &graph, nodec_scene::Scene &scene, nodec_scene::SceneEntity parent,
                        const SceneSerialization &serialization)
        : graph_{&graph},
          scene_{&scene},
          serialization_{&serialization} {
    }

    bool emplace_one() {
    }

    void emplace_all() {
        while (emplace_one()) {}
    }

private:
    const SceneSerialization *serialization_;
    const SerializableSceneGraph *graph_;
    nodec_scene::Scene *scene_;
};
} // namespace nodec_scene_serialization

#endif