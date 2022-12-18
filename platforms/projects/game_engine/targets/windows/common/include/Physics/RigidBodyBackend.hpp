#pragma once

#include <btBulletDynamicsCommon.h>

#include <memory>
#include <cassert>

class RigidBodyBackend final {
public:
    RigidBodyBackend(const btRigidBody::btRigidBodyConstructionInfo& info) {
        native_.reset(new btRigidBody(info));
    }

    ~RigidBodyBackend() {
        if (binded_world) {
            binded_world->removeRigidBody(native_.get());
        }
    }

    btRigidBody& native() {
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
    btDynamicsWorld *binded_world{nullptr};
    std::unique_ptr<btRigidBody> native_;
};