#ifndef NODEC_WORLD__IMPL__WORLD_MODULE_HPP_
#define NODEC_WORLD__IMPL__WORLD_MODULE_HPP_

#include "../world.hpp"

#include <nodec_scene/impl/scene_module.hpp>

namespace nodec_world {
namespace impl {

class WorldModule : public World {

public:
    WorldModule()
        : World{&scene_module_, initialized_.signal_interface(), stepped_.signal_interface()} {}

public:
    nodec_scene::impl::SceneModule& scene_module() {
        return scene_module_;
    }

    void step() {
        stepped_(*this);
    }

    void reset() {
        scene_module_.registry().clear();
        initialized_(*this);
    }

private:
    nodec_scene::impl::SceneModule scene_module_;
    WorldSignal initialized_;
    WorldSignal stepped_;
};
} // namespace impl
} // namespace nodec_world

#endif