#pragma once

#include <btBulletDynamicsCommon.h>

#include <nodec/macros.hpp>

#include <memory>


class Physics final {
public:
    Physics() {
        collision_config_.reset(new btDefaultCollisionConfiguration());
        dispatcher_.reset(new btCollisionDispatcher(collision_config_.get()));
        overlapping_pair_cache_.reset(new btDbvtBroadphase());
        solver_.reset(new btSequentialImpulseConstraintSolver());
        dynamics_world_.reset(new btDiscreteDynamicsWorld(dispatcher_.get(), overlapping_pair_cache_.get(), solver_.get(), collision_config_.get()));
    }

    ~Physics() {
    }

    btDynamicsWorld& dynamics_world() {
        return *dynamics_world_;
    }

private:
    std::unique_ptr<btDefaultCollisionConfiguration> collision_config_;
    std::unique_ptr<btCollisionDispatcher> dispatcher_;
    std::unique_ptr<btBroadphaseInterface> overlapping_pair_cache_;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver_;
    std::unique_ptr<btDynamicsWorld> dynamics_world_;

private:
    NODEC_DISABLE_COPY(Physics)
};