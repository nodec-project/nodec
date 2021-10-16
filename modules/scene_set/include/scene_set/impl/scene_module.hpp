#ifndef SCENE_SET__IMPL__SCENE_HPP_
#define SCENE_SET__IMPL__SCENE_HPP_

#include <scene_set/scene.hpp>

#include <scene_set/systems/hierarchy_system.hpp>


namespace scene_set {
namespace impl {

class SceneModule : public Scene {
public:
    SceneModule()
        : hierarchy_system_{ &registry_ } {
    }

    SceneRegistry& registry() {
        return registry_;
    }

    void append_child(const SceneEntity parent, const SceneEntity child) {
        hierarchy_system_.append_child(parent, child);
    }

    void remove_child(const SceneEntity parent, const SceneEntity child) {
        hierarchy_system_.remove_child(parent, child);
    }


private:
    SceneRegistry registry_;
    systems::HierarchySystem hierarchy_system_;


};


}
}


#endif