#ifndef NODEC_SCENE_SERIALIZATION__SCENE_LOADER_HPP_
#define NODEC_SCENE_SERIALIZATION__SCENE_LOADER_HPP_

#include "scene_entity_emplacer.hpp"
#include "scene_serialization.hpp"
#include "serializable_scene_graph.hpp"

#include <nodec_scene/scene.hpp>

#include <nodec/resource_management/resource_registry.hpp>

#include <cassert>

namespace nodec_scene_serialization {

class SceneLoader {
public:
    class AsyncOperation {
    public:
        AsyncOperation(const std::string &graph_name,
                       nodec_scene::SceneEntity parent,
                       nodec_scene::Scene &scene,
                       nodec_scene_serialization::EntitySerialization &serialization,
                       nodec::resource_management::ResourceRegistry &resource_registry)
            : graph_name_{graph_name},
              parent_{parent},
              scene_{&scene},
              serialization_{&serialization},
              resource_registry_{&resource_registry} {
        }

        bool is_done() {
            update();
            return state_ == State::Done;
        }

        float progress() {
            update();
            return progress_;
        }

    private:
        void update() {
            using namespace nodec::entities;

            switch (state_) {
            case State::NotStarted: {
                graph_future_ = resource_registry_->get_resource<SerializableSceneGraph>(graph_name_);
                state_ = State::GraphLoading;
            }
                return;

            case State::GraphLoading: {
                if (graph_future_.wait_for(std::chrono::seconds(0)) != std::future_status::timeout) {
                    auto graph = graph_future_.get();
                    entity_emplacer_ = std::make_unique<SceneEntityEmplacer>(graph, *scene_, parent_, *serialization_);
                    progress_ = 0.9f;
                    state_ = State::Emplacing;
                }
            }
                return;

            case State::Emplacing: {
                if (entity_emplacer_->emplace_one() == null_entity) {
                    // Nothing to emplace.
                    progress_ = 1.0f;
                    state_ = State::Done;
                }
            }
                return;

            case State::Done: return;
            }
        }

        enum class State {
            NotStarted,
            GraphLoading,
            Emplacing,
            Done
        };

        State state_{State::NotStarted};

        std::string graph_name_;
        nodec_scene::SceneEntity parent_;
        nodec_scene::Scene *scene_;
        nodec_scene_serialization::EntitySerialization *serialization_;
        nodec::resource_management::ResourceRegistry *resource_registry_;
        std::shared_future<std::shared_ptr<SerializableSceneGraph>> graph_future_;
        std::unique_ptr<SceneEntityEmplacer> entity_emplacer_;
        float progress_{0.0f};
    };

    SceneLoader(EntitySerialization &serialization,
                nodec_scene::Scene &scene,
                nodec::resource_management::ResourceRegistry &resource_registry)
        : serialization_{&serialization},
          scene_{&scene},
          resource_registry_{&resource_registry} {
    }

    AsyncOperation load_async(const std::string &graph_name, nodec_scene::SceneEntity parent = nodec::entities::null_entity) {
        return {graph_name, parent, *scene_, *serialization_, *resource_registry_};
    }

private:
    EntitySerialization *serialization_;
    nodec_scene::Scene *scene_;
    nodec::resource_management::ResourceRegistry *resource_registry_;
};

} // namespace nodec_scene_serialization
#endif
