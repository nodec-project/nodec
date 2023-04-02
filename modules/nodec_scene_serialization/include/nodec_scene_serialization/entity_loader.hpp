#ifndef NODEC_SCENE_SERIALIZATION__ENTITY_LOADER_HPP_
#define NODEC_SCENE_SERIALIZATION__ENTITY_LOADER_HPP_

#include "entity_emplacer.hpp"
#include "scene_serialization.hpp"
#include "serializable_entity.hpp"
#include "components/entity_loaded.hpp"

#include <nodec_scene/scene.hpp>

#include <nodec/macros.hpp>
#include <nodec/resource_management/resource_registry.hpp>

#include <cassert>
#include <memory>

namespace nodec_scene_serialization {

class EntityLoader {
public:
    class Task {
    public:
        enum class State {
            NotStarted,
            ResourceLoading,
            Done,
        };

        Task(const std::string &source, const nodec_scene::SceneEntity &target,
             const SceneSerialization &serialization, nodec_scene::Scene &scene, nodec::resource_management::ResourceRegistry &resource_registry)
            : serialization_(serialization),
              scene_(scene),
              resource_registry_(resource_registry),
              source_(source), target_(target) {}

        void update() {
            switch (state_) {
            case State::NotStarted:
                entity_future_ = resource_registry_.get_resource_async<SerializableEntity>(source_);
                state_ = State::ResourceLoading;
                break;

            case State::ResourceLoading:
                if (entity_future_.wait_for(std::chrono::seconds(0)) != std::future_status::timeout) {
                    auto ser_entity = entity_future_.get();
                    EntityEmplacer(serialization_).emplace(ser_entity.get(), target_, scene_);
                    scene_.registry().emplace_component<components::EntityLoaded>(target_);
                    state_ = State::Done;
                }
                break;

            case State::Done:
                break;
            }
        }

        State state() const noexcept {
            return state_;
        }

        // nodec_scene::SceneEntity result() const noexcept {
        //     assert(state_ == State::Done);
        //     return result_;
        // }

    private:
        const SceneSerialization &serialization_;
        nodec_scene::Scene &scene_;
        nodec::resource_management::ResourceRegistry &resource_registry_;
        std::string source_;
        nodec_scene::SceneEntity target_;
        // nodec_scene::SceneEntity result_;
        State state_{State::NotStarted};
        std::shared_future<std::shared_ptr<SerializableEntity>> entity_future_;

    private:
        NODEC_DISABLE_COPY(Task)
    };

    class AsyncOperation {
    public:
        AsyncOperation(std::shared_ptr<Task> task)
            : task_(task) {
        }

        AsyncOperation(AsyncOperation &&other)
            : task_(std::move(other.task_)) {
        }

        AsyncOperation &operator=(AsyncOperation &&other) noexcept {
            task_ = std::move(other.task_);
            return *this;
        }

        void result() const {
            while (task_->state() != Task::State::Done) task_->update();
            // return task_->result();
        }

        bool is_done() const {
            return task_->state() == Task::State::Done;
        }

        // TODO: progress()

    private:
        std::shared_ptr<Task> task_;

    private:
        NODEC_DISABLE_COPY(AsyncOperation)
    };

public:
    virtual void load_direct(const std::string &source,
                             const nodec_scene::SceneEntity &target) = 0;

    virtual AsyncOperation load_async(const std::string &source,
                                      const nodec_scene::SceneEntity &target) = 0;
};

} // namespace nodec_scene_serialization
#endif
