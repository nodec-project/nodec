#pragma once

#include "RigidBodyMotionState.hpp"

#include <nodec_physics/components/physics_shape.hpp>
#include <nodec_physics/components/rigid_body.hpp>
#include <nodec/math/math.hpp>
#include <nodec_bullet3_compat/nodec_bullet3_compat.hpp>

#include <btBulletDynamicsCommon.h>

#include <algorithm>
#include <cassert>
#include <memory>

class RigidBodyBackend final {
public:
    RigidBodyBackend(float mass,
                     const nodec_physics::components::PhysicsShape &shape,
                     const nodec::Vector3f &world_shape_scale)
        : world_shape_scale_(world_shape_scale) {
        using namespace nodec_physics::components;
        // const btVector3 &local_inertia = btVector3(0, 0, 0)

        switch (shape.shape_type) {
        case PhysicsShape::ShapeType::Box: {
            // Make the unit box shape. Then set the size using SetLocalScaling().
            collision_shape_.reset(new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)));
            const auto size = world_shape_scale_ * shape.size;
            collision_shape_->setLocalScaling(btVector3(size.x, size.y, size.z));
        } break;

        case PhysicsShape::ShapeType::Sphere: {
            collision_shape_.reset(new btSphereShape(1.f));
            const auto radius = std::max({world_shape_scale.x, world_shape_scale.y, world_shape_scale.z}) * shape.radius;
            collision_shape_->setLocalScaling(btVector3(radius, radius, radius));
        } break;

        // Nothing to do here.
        default: break;
        }

        btVector3 local_inertia(0, 0, 0);
        collision_shape_->calculateLocalInertia(mass, local_inertia);

        motion_state_.reset(new RigidBodyMotionState());
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

    void update_transform(const nodec::Vector3f& world_position, const nodec::Quaternionf& world_rotation) {
        using namespace nodec;

        //btTransform rb_trfm;
        //motion_state_->getWorldTransform(rb_trfm);
        const auto &rb_trfm = native_->getWorldTransform();

        const auto rb_position = static_cast<Vector3f>(to_vector3(rb_trfm.getOrigin()));
        const auto rb_rotation = static_cast<Quaternionf>(to_quatenion(rb_trfm.getRotation()));

        if (math::approx_equal(world_position, rb_position) && math::approx_equal_rotation(world_rotation, rb_rotation, math::default_rel_tol<float>, 0.001f)) return;

        btTransform rb_trfm_updated;
        rb_trfm_updated.setIdentity();
        rb_trfm_updated.setOrigin(btVector3(world_position.x, world_position.y, world_position.z));
        rb_trfm_updated.setRotation(btQuaternion(world_rotation.x, world_rotation.y, world_rotation.z, world_rotation.w));
        native_->setWorldTransform(rb_trfm_updated);

        native_->activate();
    }

private:
    std::unique_ptr<RigidBodyMotionState> motion_state_;
    std::unique_ptr<btCollisionShape> collision_shape_;
    std::unique_ptr<btRigidBody> native_;
    btDynamicsWorld *binded_world{nullptr};
    nodec::Vector3f world_shape_scale_;
};