#ifndef NODEC_SCENE__COMPONENTS__LOCAL_TO_WORLD_HPP_
#define NODEC_SCENE__COMPONENTS__LOCAL_TO_WORLD_HPP_

#include <nodec/matrix4x4.hpp>

namespace nodec_scene {
namespace components {

struct LocalToWorld {
    nodec::Matrix4x4f value;

    /**
     * @brief Dirty flag. If you modify the value, you must set dirty flag to be
     * true.
     *
     * The default value is false to prevent LocalToWorld, which is
     * automatically added by assigning LocalTransform, from overwriting
     * LocalTransform's transform.
     */
    bool dirty{false};
};

} // namespace components
} // namespace nodec_scene

#endif