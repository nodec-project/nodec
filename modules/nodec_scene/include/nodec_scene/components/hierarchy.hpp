#ifndef NODEC_SCENE__COMPONENTS__HIERARCHY_HPP_
#define NODEC_SCENE__COMPONENTS__HIERARCHY_HPP_

#include "../scene_registry.hpp"

namespace nodec_scene {
namespace components {

struct Hierarchy {
    //! The number of children.
    std::size_t child_count{0};

    //! The entity identifier for the first child, if any.
    SceneEntity first{nodec::entities::null_entity};

    SceneEntity last{nodec::entities::null_entity};

    //! The previous sibling in the list of children for the parent.
    SceneEntity prev{nodec::entities::null_entity};

    //! The next sibling in the list of children for the parent.
    SceneEntity next{nodec::entities::null_entity};

    //! The entity identifier of the parent, if any.
    SceneEntity parent{nodec::entities::null_entity};
};

} // namespace components
} // namespace nodec_scene

#endif