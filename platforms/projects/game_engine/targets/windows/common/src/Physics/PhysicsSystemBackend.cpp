#include <Physics/PhysicsSystemBackend.hpp>
#include <Physics/RigidBodyActivity.hpp>
#include <Physics/RigidBodyBackend.hpp>

#include <nodec_physics/components/physics_shape.hpp>
#include <nodec_physics/components/rigid_body.hpp>

#include <nodec/logging.hpp>
#include <nodec/math/gfx.hpp>

bool operator==(const btVector3& left, const nodec::Vector3f& right) {
    return left.x() == right.x && left.y() == right.y && left.z() == right.z;
}

bool operator!=(const btVector3 &left, const nodec::Vector3f &right) {
    return !(left == right);
}


void PhysicsSystemBackend::on_stepped(nodec_world::World &world) {
    using namespace nodec;
    using namespace nodec_scene;
    using namespace nodec_scene::components;
    using namespace nodec_physics::components;

    // Pre process for step of simulation.
    world.scene().registry().view<RigidBody, PhysicsShape, Transform>().each(
        [&](auto entt, RigidBody &rigid_body, PhysicsShape &shape, Transform &trfm) {
            Vector3f world_position;
            Quaternionf world_rotation;
            Vector3f world_scale;

            math::gfx::decompose_trs(trfm.local2world, world_position, world_rotation, world_scale);

            RigidBodyActivity *activity;
            {
                // Emplace an activity component, if not.
                auto result = world.scene().registry().emplace_component<RigidBodyActivity>(entt);
                activity = &result.first;

                if (result.second) {
                    // When the activity is first created.

                    auto rigid_body_backend = std::make_unique<RigidBodyBackend>(rigid_body.mass, shape, world_scale);

                    rigid_body_backend->bind_world(*dynamics_world_);
                    activity->rigid_body_backend = std::move(rigid_body_backend);
                }
            }

            // Sync entity -> bullet rigid body.

            {

                btTransform rb_trfm;
                activity->rigid_body_backend->native().getMotionState()->getWorldTransform(rb_trfm);

                if (activity->prev_world_position != world_position || activity->prev_world_rotation != world_rotation) {
                    btTransform rb_trfm_updated;
                    rb_trfm_updated.setIdentity();
                    rb_trfm_updated.setOrigin(btVector3(world_position.x, world_position.y, world_position.z));
                    rb_trfm_updated.setRotation(btQuaternion(world_rotation.x, world_rotation.y, world_rotation.z, world_rotation.w));
                    activity->rigid_body_backend->native().setWorldTransform(rb_trfm_updated);

                    activity->rigid_body_backend->native().activate(true);
                }

                // activity->rigid_body_backend->native().getMotionState()->setWorldTransform(rb_trfm);
                // activity->rigid_body_backend->native().translate();

                activity->prev_world_position = world_position;
                activity->prev_world_rotation = world_rotation;
            }
        });

    {
        std::vector<SceneEntity> to_deleted;

        world.scene().registry().view<RigidBodyActivity>().each([&](auto entt, RigidBodyActivity &activity) {
            if (world.scene().registry().all_of<RigidBody, PhysicsShape>(entt)) {
                return;
            }

            to_deleted.push_back(entt);
        });

        for (auto &entt : to_deleted) {
            world.scene().registry().remove_component<RigidBodyActivity>(entt);
        }
    }

    dynamics_world_->stepSimulation(world.clock().delta_time(), 10);

    // Sync rigid body -> entity.
    world.scene().registry().view<RigidBody, RigidBodyActivity, Transform>().each(
        [&](auto entt, RigidBody &rigid_body, RigidBodyActivity &activity, Transform &trfm) {
            if (rigid_body.mass == 0.0f) return;

            btTransform rb_trfm;
            activity.rigid_body_backend->native().getMotionState()->getWorldTransform(rb_trfm);

            Vector3f world_position(rb_trfm.getOrigin().getX(), rb_trfm.getOrigin().getY(), rb_trfm.getOrigin().getZ());
            Quaternionf world_rotation(
                rb_trfm.getRotation().getX(),
                rb_trfm.getRotation().getY(),
                rb_trfm.getRotation().getZ(),
                rb_trfm.getRotation().getW());

            auto delta_position = world_position - activity.prev_world_position;
            auto delta_rotation = math::conj(activity.prev_world_rotation) * world_rotation;

            trfm.local_position += delta_position;
            trfm.local_rotation = delta_rotation * trfm.local_rotation;

            trfm.dirty = true;
        });
}
