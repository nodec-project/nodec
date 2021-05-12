#ifndef SCENE_SET__SYSTEMS__IMPL__IMPL_HIERARCHY_SYSTEM_HPP_
#define SCENE_SET__SYSTEMS__IMPL__IMPL_HIERARCHY_SYSTEM_HPP_

#include<scene_set/systems/hierarchy_system.hpp>

#include<scene_set/scene_registry.hpp>

namespace scene_set {
namespace systems {
namespace impl {


struct HierarchySystemData {
    using RegistryConnection = signals::SignalInterface<void(SceneRegistry&, const SceneEntity)>::Connection;

    //RegistryConnection 
};

void init_hierarchy_system(SceneRegistry& registry);

}
}
}

#endif