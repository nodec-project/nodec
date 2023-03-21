#ifndef NODEC_SCENE_SERIALIZATION__IMPL__ENTITY_LOADER_IMPL_HPP_
#define NODEC_SCENE_SERIALIZATION__IMPL__ENTITY_LOADER_IMPL_HPP_

#include "../entity_loader.hpp"
#include "../scene_serialization.hpp"

#include <nodec/resource_management/resource_registry.hpp>

#include <list>

namespace nodec_scene_serialization {
namespace impl {

class EntityLoaderImpl final : public EntityLoader {
public:
    EntityLoaderImpl(SceneSerialization &serialization,
                     nodec_scene::Scene &scene,
                     nodec::resource_management::ResourceRegistry &resource_registry)
        : serialization_{serialization},
          scene_{scene},
          resource_registry_{resource_registry} {
    }

    nodec_scene::SceneEntity load_direct(const std::string &source,
                                         const nodec_scene::SceneEntity &parent = nodec::entities::null_entity) override {
        using namespace nodec::entities;
        auto ser_entity = resource_registry_.get_resource_direct<SerializableEntity>(source);
        if (!ser_entity) return null_entity;

        return EntityEmplacer(serialization_).emplace(ser_entity.get(), parent, scene_);
    }

    AsyncOperation load_async(const std::string &source, const nodec_scene::SceneEntity &parent = nodec::entities::null_entity) override {
        auto task = std::make_shared<Task>(source, parent, serialization_, scene_, resource_registry_);
        tasks_.push_back(task);
        return {task};
    }

    void update() {
        for (auto iter = tasks_.begin(); iter != tasks_.end();) {
            if ((*iter)->state() == Task::State::Done) {
                iter = tasks_.erase(iter);
                continue;
            }

            (*iter)->update();
            ++iter;
        }
    }

private:
    const SceneSerialization &serialization_;
    nodec_scene::Scene &scene_;
    nodec::resource_management::ResourceRegistry &resource_registry_;
    std::list<std::shared_ptr<Task>> tasks_;
};

} // namespace impl
} // namespace nodec_scene_serialization

#endif