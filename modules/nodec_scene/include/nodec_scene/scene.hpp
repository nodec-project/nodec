#ifndef NODEC_SCENE__SCENE_HPP_
#define NODEC_SCENE__SCENE_HPP_

#include "components/hierarchy.hpp"
#include "components/local_to_world.hpp"
#include "components/name.hpp"
#include "components/local_transform.hpp"
#include "scene_registry.hpp"
#include "systems/hierarchy_system.hpp"

#include <nodec/entities/registry.hpp>
#include <nodec/macros.hpp>

namespace nodec_scene {

class Scene {
public:
    Scene()
        : hierarchy_system_{&registry_} {
        using namespace components;

        hierarchy_system_.hierarchy_changed().connect([&](SceneEntity parent, SceneEntity child) {
            auto *trfm = registry_.try_get_component<LocalTransform>(child);
            if (trfm == nullptr) return;
            trfm->dirty = true;
        });

        // NOTE: No case about disconnecting of signal.
        //  Because the registry is owned by this.
        registry_.component_constructed<LocalTransform>().connect([](SceneRegistry &registry, SceneEntity entity) {
            registry.emplace_component<LocalToWorld>(entity);
        });
    }

    virtual ~Scene() {}

    SceneRegistry &registry() noexcept {
        return registry_;
    }

    const SceneRegistry &registry() const noexcept {
        return registry_;
    }

    systems::HierarchySystem &hierarchy_system() noexcept {
        return hierarchy_system_;
    }

    const systems::HierarchySystem &hierarchy_system() const noexcept {
        return hierarchy_system_;
    }

    SceneEntity create_entity(const std::string &name) {
        using namespace components;

        auto entity = registry_.create_entity();

        registry_.emplace_component<Name>(entity, name);
        registry_.emplace_component<Hierarchy>(entity);
        registry_.emplace_component<LocalTransform>(entity);

        return entity;
    }

    SceneEntity create_entity() {
        using namespace components;

        auto entity = registry_.create_entity();
        registry_.emplace_component<Hierarchy>(entity);
        registry_.emplace_component<LocalTransform>(entity);

        return entity;
    }

private:
    SceneRegistry registry_;
    systems::HierarchySystem hierarchy_system_;

private:
    NODEC_DISABLE_COPY(Scene)
};

} // namespace nodec_scene

#endif