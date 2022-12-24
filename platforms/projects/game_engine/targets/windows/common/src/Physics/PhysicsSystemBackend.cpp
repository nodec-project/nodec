#include <Physics/PhysicsSystemBackend.hpp>
#include <Physics/RigidBodyActivity.hpp>
#include <Physics/RigidBodyBackend.hpp>

#include <nodec_physics/components/physics_shape.hpp>

#include <nodec/logging.hpp>
#include <nodec/math/gfx.hpp>

namespace {

std::unique_ptr<btCollisionShape> make_collision_shape(const nodec_physics::components::PhysicsShape &shape) {
    using namespace nodec_physics::components;

    switch (shape.shape_type) {
    case PhysicsShape::ShapeType::Box: {
        return std::make_unique<btBoxShape>(btVector3(shape.size.x / 2.f, shape.size.y / 2.f, shape.size.z / 2.f));
    } break;

    case PhysicsShape::ShapeType::Sphere: {
    } break;

    // Nothing to do here.
    default: break;
    }
    return nullptr;
}

} // namespace

void PhysicsSystemBackend::on_stepped(nodec_world::World &world) {
    using namespace nodec;
    using namespace nodec_scene;
    using namespace nodec_scene::components;
    using namespace nodec_physics::components;

    // Pre process for step of simulation.
    world.scene().registry().view<RigidBody, PhysicsShape, Transform>().each(
        [&](auto entt, RigidBody &rigid_body, PhysicsShape &shape, Transform &trfm) {
            RigidBodyActivity *activity;
            {
                // Emplace an activity component, if not.
                auto result = world.scene().registry().emplace_component<RigidBodyActivity>(entt);
                activity = &result.first;

                if (result.second) {
                    // When the activity is first created.

                    auto collision_shape = make_collision_shape(shape);
                    assert(collision_shape);

                    auto motion_state = std::make_unique<btDefaultMotionState>();

                    auto rigid_body_backend = std::make_unique<RigidBodyBackend>(rigid_body.mass,
                                                                                 std::move(motion_state),
                                                                                 std::move(collision_shape));

                    rigid_body_backend->bind_world(*dynamics_world_);
                    activity->rigid_body_backend = std::move(rigid_body_backend);
                }
            }

            // Sync entity -> bullet rigid body.

            Vector3f translation;
            Quaternionf rotation;
            Vector3f scale;

            math::gfx::decompose_trs(trfm.local2world, translation, rotation, scale);

            btTransform rb_trfm;
            rb_trfm.setIdentity();
            rb_trfm.setOrigin(btVector3(translation.x, translation.y, translation.z));
            rb_trfm.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
            activity->rigid_body_backend->native().getMotionState()->setWorldTransform(rb_trfm);
        });

    {
        std::vector<SceneEntity> to_deleted;

        world.scene().registry().view<RigidBodyActivity>().each([&](auto entt, RigidBodyActivity &activity) {
            //  * Finalize btRigidBody.

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

    // Sync with transform.
    world.scene().registry().view<RigidBody, RigidBodyActivity, Transform>().each(
        [&](auto entt, RigidBody &rigid_body, RigidBodyActivity &activity, Transform &trfm) {
            btTransform rb_trfm;
            activity.rigid_body_backend->native().getMotionState()->getWorldTransform(rb_trfm);
            logging::InfoStream(__FILE__, __LINE__) << rb_trfm.getOrigin().getY();
        });
}
