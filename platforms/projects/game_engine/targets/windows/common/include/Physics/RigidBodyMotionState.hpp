#pragma once

#include <btBulletDynamicsCommon.h>

class RigidBodyMotionState final : public btMotionState {
public:
    RigidBodyMotionState()
        : current_trfm_(current_trfm_) {}

    /// synchronizes world transform from user to physics
    void getWorldTransform(btTransform &trfm) const override {
        trfm = current_trfm_;
    }

    /// synchronizes world transform from physics to user
    /// Bullet only calls the update of worldtransform for active objects
    void setWorldTransform(const btTransform &trfm) override {
        current_trfm_ = trfm;
    }

private:
    btTransform current_trfm_;
};