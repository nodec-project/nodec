#ifndef NODEC_PHYSICS__SYSTEMS__PHYSICS_SYSTEM_HPP_
#define NODEC_PHYSICS__SYSTEMS__PHYSICS_SYSTEM_HPP_

#include "../collision_info.hpp"

#include <nodec/macros.hpp>
#include <nodec/signals/signal.hpp>

namespace nodec_physics {

namespace systems {

class PhysicsSystem {
public:
    using CollisionSignal = nodec::signals::Signal<void(const CollisionInfo &)>;

    PhysicsSystem(CollisionSignal &collision_stay_signal)
        : collision_stay_signal_(collision_stay_signal) {}

    virtual ~PhysicsSystem() {}

    // bool raycast(rayStart, rayEnd, RaycastHit)

    // void set_gravity(const nodec::Vectro3f &gravity);

    // nodec::Vector3f gravity();

    CollisionSignal::SignalInterface collision_stay() {
        return collision_stay_signal_.signal_interface();
    }

private:
    CollisionSignal &collision_stay_signal_;

private:
    NODEC_DISABLE_COPY(PhysicsSystem)
};

} // namespace systems
} // namespace nodec_physics
#endif