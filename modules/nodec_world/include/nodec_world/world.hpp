#ifndef NODEC_WORLD__WORLD_HPP_
#define NODEC_WORLD__WORLD_HPP_

#include "world_clock.hpp"

#include <nodec_scene/scene.hpp>

#include <nodec/macros.hpp>
#include <nodec/signals/signal.hpp>

namespace nodec_world {

class World {
public:
    using WorldSignal = nodec::signals::Signal<void(World &)>;

    World(WorldSignal &initialized_signal,
          WorldSignal &stepped_signal,
          WorldClock::WorldClockInterface clock_interface)
        : initialized_{initialized_signal},
          stepped_{stepped_signal},
          clock_{clock_interface} {}

    virtual ~World() {}

    nodec_scene::Scene &scene() {
        return scene_;
    }

    const nodec_scene::Scene &scene() const {
        return scene_;
    }

    // About scene (world) lifecycle.
    //  * https://docs.unity3d.com/Packages/com.unity.entities@0.17/manual/world.html
    //  * https://www.gymlibrary.ml/content/environment_creation/

    WorldSignal::SignalInterface initialized() {
        return initialized_.signal_interface();
    }

    WorldSignal::SignalInterface stepped() {
        return stepped_.signal_interface();
    }

    const WorldClock::WorldClockInterface &clock() const noexcept {
        return clock_;
    }

    WorldClock::WorldClockInterface &clock() noexcept {
        return clock_;
    }

private:
    nodec_scene::Scene scene_;
    WorldSignal& initialized_;
    WorldSignal& stepped_;
    WorldClock::WorldClockInterface clock_;

    NODEC_DISABLE_COPY(World)
};
} // namespace nodec_world

#endif