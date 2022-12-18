#pragma once

#include "RigidBodyBackend.hpp"

struct RigidBodyActivity {

    std::unique_ptr<RigidBodyBackend> rigid_body_backend;

};