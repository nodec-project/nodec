#ifndef NODEC_SCENE_EDITOR__SELECTION_HPP_
#define NODEC_SCENE_EDITOR__SELECTION_HPP_

#include <nodec_scene/scene_registry.hpp>

#include <nodec/signals/signal.hpp>

namespace nodec_scene_editor {

class Selection {
    using SceneEntity = nodec_scene::SceneEntity;

public:
    SceneEntity active_scene_entity() const noexcept {
        return active_scene_entity_;
    }

    void select(SceneEntity entity) {
        if (entity == active_scene_entity_) {
            return;
        }

        active_scene_entity_ = entity;
        active_scene_entity_changed_(entity);
    }

    decltype(auto) active_scene_entity_changed() {
        return active_scene_entity_changed_.signal_interface();
    }

private:
    SceneEntity active_scene_entity_{nodec::entities::null_entity};
    nodec::signals::Signal<void(SceneEntity)> active_scene_entity_changed_;
};

} // namespace nodec_scene_editor

#endif