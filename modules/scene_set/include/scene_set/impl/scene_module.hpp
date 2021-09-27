#ifndef SCENE_SET__IMPL__SCENE_HPP_
#define SCENE_SET__IMPL__SCENE_HPP_

#include <scene_set/scene.hpp>

#include <scene_set/systems/impl/impl_hierarchy_system.hpp>


namespace scene_set {
namespace impl {

class SceneModule : public Scene {
public:
    SceneModule() {
        hierarchySystem.init(registry_);
    }

private:
    systems::impl::HierarchySystem hierarchySystem;


};


}
}


#endif