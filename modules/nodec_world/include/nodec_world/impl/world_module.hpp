#ifndef NODEC_WORLD__IMPL__WORLD_MODULE_HPP_
#define NODEC_WORLD__IMPL__WORLD_MODULE_HPP_

#include "../world.hpp"

namespace nodec_world {
namespace impl {

class WorldModule : public World {
public:
    WorldModule()
        : World{initialized_.signal_interface(), stepped_.signal_interface(), clock_.clock_interface()} {}

public:
    void step() {
        clock_.tick();
        stepped_(*this);
    }

    void step(float delta_sec) {
        clock_.tick(delta_sec);
        stepped_(*this);
    }

    void reset() {
        scene().registry().clear();
        clock_.reset();
        initialized_(*this);
    }

private:
    WorldSignal initialized_;
    WorldSignal stepped_;
    WorldClock clock_;
};
} // namespace impl
} // namespace nodec_world

#endif