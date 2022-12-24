#pragma once

#include <btBulletDynamicsCommon.h>

#include <cassert>
#include <memory>

class RigidBodyBackend final {
public:
    RigidBodyBackend(float mass,
                     std::unique_ptr<btMotionState> motion_state,
                     std::unique_ptr<btCollisionShape> collision_shape,
                     const btVector3 &local_inertia = btVector3(0, 0, 0))
        : motion_state_(std::move(motion_state)),
          collision_shape_(std::move(collision_shape)) {
        native_.reset(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, motion_state_.get(), collision_shape_.get(), local_inertia)));
    }

    ~RigidBodyBackend() {
        if (binded_world) {
            binded_world->removeRigidBody(native_.get());
        }
    }

    btRigidBody &native() {
        return *native_;
    }

    const btRigidBody &native() const {
        return *native_;
    }

    void bind_world(btDynamicsWorld &world) {
        world.addRigidBody(native_.get());
        binded_world = &world;
    }

    void unbind_world(btDynamicsWorld &world) {
        assert(&world == binded_world);
        binded_world->removeRigidBody(native_.get());
    }

private:
    std::unique_ptr<btMotionState> motion_state_;
    std::unique_ptr<btCollisionShape> collision_shape_;
    std::unique_ptr<btRigidBody> native_;
    btDynamicsWorld *binded_world{nullptr};
};