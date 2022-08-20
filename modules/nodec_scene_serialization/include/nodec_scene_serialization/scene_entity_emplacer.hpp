#ifndef NODEC_SCENE_SERIALIZATION__SCENE_ENTITIY_EMPLACER_HPP_
#define NODEC_SCENE_SERIALIZATION__SCENE_ENTITIY_EMPLACER_HPP_

#include "scene_serialization.hpp"
#include "serializable_scene_graph.hpp"

#include <nodec_scene/components/basic.hpp>
#include <nodec_scene/scene.hpp>

#include <cassert>
#include <stack>

namespace nodec_scene_serialization {

/**
 * @brief The SceneEntityEmplacer class allows emplacing the nodes of graph to the scene as new scene entity.
 *
 */
class SceneEntityEmplacer {
    using NodePtr = std::shared_ptr<SerializableEntityNode>;
    using NodePtrIterator = std::vector<NodePtr>::const_iterator;

    struct EmplacementData {
        NodePtr node;
        nodec_scene::SceneEntity parent;
    };

public:
    SceneEntityEmplacer(std::shared_ptr<const SerializableSceneGraph> graph, nodec_scene::Scene &scene, nodec_scene::SceneEntity parent,
                        const SceneSerialization &serialization)
        : graph_{graph},
          scene_{&scene},
          serialization_{&serialization} {
        if (graph) {
            for (auto node : graph->roots) {
                stack_.push({node, parent});
            }
        }
    }

    bool emplace_one() {
        using namespace nodec::entities;

        if (stack_.empty()) return false;
        auto emplacement = std::move(stack_.top());
        stack_.pop();

        auto entity = serialization_->make_entity(emplacement.node, scene_->registry());

        assert(entity != null_entity);

        if (emplacement.parent == null_entity) {
            // if no parent, just attach the Hierarchy component as root entity.
            scene_->registry().emplace_component<nodec_scene::components::Hierarchy>(entity);
        } else {
            scene_->append_child(emplacement.parent, entity);
        }

        for (auto child : emplacement.node->children) {
            stack_.push({child, entity});
        }
        return true;
    }

    void emplace_all() {
        while (emplace_one()) {}
    }

private:
    const SceneSerialization *serialization_;
    std::shared_ptr<const SerializableSceneGraph> graph_;
    nodec_scene::Scene *scene_;
    std::stack<EmplacementData> stack_;
};
} // namespace nodec_scene_serialization

#endif