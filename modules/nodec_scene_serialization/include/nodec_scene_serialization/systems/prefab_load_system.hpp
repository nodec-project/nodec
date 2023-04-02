#ifndef NODEC_SCENE_SERIALIZATION__SYSTEMS__PREFAB_LOAD_SYSTEM_HPP_
#define NODEC_SCENE_SERIALIZATION__SYSTEMS__PREFAB_LOAD_SYSTEM_HPP_

#include "../components/entity_built.hpp"
#include "../components/prefab.hpp"
#include "../entity_loader.hpp"

#include <nodec/optional.hpp>
#include <nodec/signals/signal.hpp>
#include <nodec_scene/scene.hpp>

namespace nodec_scene_serialization {
namespace systems {

class PrefabLoadSystem final {
public:
    struct PrefabLoadActivity {
        nodec::optional<EntityLoader::AsyncOperation> operation;
    };

    PrefabLoadSystem(nodec_scene::Scene &scene, EntityLoader &entity_loader)
        : scene_(scene), entity_loader_(entity_loader) {
        using namespace components;
        using namespace nodec_scene;

        on_prefab_constructed_ = scene_.registry().component_constructed<Prefab>().connect([=](SceneRegistry &registry, const SceneEntity &entity) {
            registry.emplace_component<PrefabLoadActivity>(entity);
        });

        on_prefab_destroyed_ = scene_.registry().component_destroyed<Prefab>().connect([=](SceneRegistry &registry, const SceneEntity &entity) {
            registry.remove_component<PrefabLoadActivity>(entity);
        });
    }

    void update() {
        using namespace components;
        using namespace nodec_scene;

        {
            // Entities already built with EntityBuilder should not be loaded newly.
            auto view = scene_.registry().view<PrefabLoadActivity, EntityBuilt>();
            scene_.registry().remove_component<PrefabLoadActivity>(view.begin(), view.end());
        }

        std::vector<SceneEntity> to_deletes;

        scene_.registry().view<Prefab, PrefabLoadActivity>().each([&](const SceneEntity &entity, Prefab &prefab, PrefabLoadActivity &activity) {
            if (activity.operation) {
                // Currently loading.
                if (!activity.operation->is_done()) return;

                to_deletes.emplace_back(entity);
                return;
            }

            if (prefab.active) {
                // start to load.
                activity.operation = entity_loader_.load_async(prefab.source, entity);
                return;
            }
        });

        scene_.registry().remove_component<PrefabLoadActivity>(to_deletes.begin(), to_deletes.end());
    }

private:
    nodec_scene::Scene &scene_;
    EntityLoader &entity_loader_;
    nodec::signals::Connection on_prefab_constructed_;
    nodec::signals::Connection on_prefab_destroyed_;
};
} // namespace systems
} // namespace nodec_scene_serialization

#endif