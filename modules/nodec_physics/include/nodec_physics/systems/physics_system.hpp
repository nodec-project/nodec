#ifndef NODEC_PHYSICS__SYSTEMS__PHYSICS_SYSTEM_HPP_
#define NODEC_PHYSICS__SYSTEMS__PHYSICS_SYSTEM_HPP_

#include <nodec/macros.hpp>

namespace nodec_physics {

namespace systems {

class PhysicsSystem {
public:
    PhysicsSystem() {}
    virtual ~PhysicsSystem() {}

    // bool raycast(rayStart, rayEnd, RaycastHit)

    // void set_gravity(const nodec::Vectro3f &gravity);

    // nodec::Vector3f gravity();


private:
    NODEC_DISABLE_COPY(PhysicsSystem)
};


}
} // namespace nodec_physics
#endif