#ifndef NODEC_SCENE__SCENE_HPP_
#define NODEC_SCENE__SCENE_HPP_

#include "components/hierarchy.hpp"
#include "components/name.hpp"
#include "components/transform.hpp"
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

        hierarchy_system_.hierarchy_changed().connect([&](auto parent, auto child) {
            auto *trfm = registry_.try_get_component<Transform>(child);
            if (trfm == nullptr) return;
            trfm->dirty = true;
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
        using namespace nodec_scene::components;

        auto entity = registry_.create_entity();

        registry_.emplace_component<Name>(entity, name);
        registry_.emplace_component<Hierarchy>(entity);
        registry_.emplace_component<Transform>(entity);

        return entity;
    }

    SceneEntity create_entity() {
        using namespace nodec_scene::components;

        auto entity = registry_.create_entity();
        registry_.emplace_component<Hierarchy>(entity);
        registry_.emplace_component<Transform>(entity);

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