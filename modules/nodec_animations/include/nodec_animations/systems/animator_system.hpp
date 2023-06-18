#ifndef NODEC_ANIMATIONS__SYSTEMS__ANIMATOR_SYSTEM_HPP_
#define NODEC_ANIMATIONS__SYSTEMS__ANIMATOR_SYSTEM_HPP_

#include <nodec_scene/scene_registry.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>

#include "../components/animator.hpp"

namespace nodec_animations {
namespace systems {

namespace internal {

}

class AnimatorSystem {
public:
    AnimatorSystem(nodec_scene_serialization::SceneSerialization &serialization)
        : serialization_(serialization) {}

    void update(nodec_scene::SceneRegistry &registry) {
        using namespace nodec_scene;
        using namespace components;

        {
            auto view = registry.view<Animator, AnimatorBegin>();

            view.each([](SceneEntity entity, Animator &animator, AnimatorBegin &) {
                if (!animator.clip) return;
            });

            registry.remove_component<AnimatorBegin>(view.begin(), view.end());
        }

        registry.view<Animator>().each([](SceneEntity entity, Animator &animator) {
            if (!animator.clip) return;
        });
    }

    void bind(components::Animator &animator, nodec_scene::SceneRegistry &registry, const nodec_scene::SceneEntity &entity) {
        if (!animator.clip) return;
    }

private:
    nodec_scene_serialization::SceneSerialization &serialization_;
};
} // namespace systems
} // namespace nodec_animations
#endif