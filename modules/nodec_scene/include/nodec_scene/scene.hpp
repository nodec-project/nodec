#ifndef NODEC_SCENE__SCENE_HPP_
#define NODEC_SCENE__SCENE_HPP_

#include "scene_registry.hpp"

#include <nodec/entities/registry.hpp>
#include <nodec/macros.hpp>
#include <nodec/signals.hpp>

namespace nodec_scene {

class Scene {
public:
    Scene() {}
    virtual ~Scene() {}

    virtual SceneRegistry &registry() noexcept = 0;

    virtual void append_child(const SceneEntity parent, const SceneEntity child) = 0;
    virtual void remove_child(const SceneEntity parent, const SceneEntity child) = 0;

    virtual const std::vector<SceneEntity> &root_entites() const noexcept = 0;

    virtual SceneEntity create_entity(const std::string &name) = 0;

public:
    // // About scene (world) lifecycle.
    // //  * https://docs.unity3d.com/Packages/com.unity.entities@0.17/manual/world.html
    // //  * https://www.gymlibrary.ml/content/environment_creation/
    // using SceneSignal = nodec::signals::Signal<void(Scene &)>;

    // virtual SceneSignal::SignalInterface initialized() = 0;
    // virtual SceneSignal::SignalInterface stepped() = 0;

private:
    NODEC_DISABLE_COPY(Scene);
};

} // namespace nodec_scene

#endif