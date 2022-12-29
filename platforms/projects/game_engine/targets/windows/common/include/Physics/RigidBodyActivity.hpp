#pragma once

#include "RigidBodyBackend.hpp"

#include <nodec/quaternion.hpp>
#include <nodec/vector3.hpp>

struct RigidBodyActivity {
    std::unique_ptr<RigidBodyBackend> rigid_body_backend;
    nodec::Vector3f prev_world_position;
    nodec::Quaternionf prev_world_rotation;
};