#ifndef NODEC_PHYSICS__COMPONENTS__CENTRAL_FORCE_HPP_
#define NODEC_PHYSICS__COMPONENTS__CENTRAL_FORCE_HPP_

#include <nodec/vector3.hpp>

namespace nodec_physics {
namespace components {

struct CentralForce {
    nodec::Vector3f force;
};

} // namespace components
} // namespace nodec_physics

#endif