#include <Physics/PhysicsSystemBackend.hpp>
#include <Physics/RigidBodyActivity.hpp>
#include <Physics/RigidBodyBackend.hpp>

#include <nodec_physics/components/physics_shape.hpp>

#include <nodec/logging.hpp>

namespace {

btCollisionShape* make_collision_shape(const nodec_physics::components::PhysicsShape& shape) {
    using namespace nodec_physics::components;

    switch (shape.shape_type) {
    case PhysicsShape::ShapeType::Box: {
        return new btBoxShape(btVector3(shape.size.x / 2.f, shape.size.y / 2.f, shape.size.z / 2.f));
    } break;

    case PhysicsShape::ShapeType::Sphere: {
    } break;

    }
    return nullptr;
}

}

void PhysicsSystemBackend::on_stepped(nodec_world::World &world) {
    using namespace nodec_scene;
    using namespace nodec_scene::components;
    using namespace nodec_physics::components;

    return;
    // TODO: Implement.

    // View RigidBody, PhysicsShape
    world.scene().registry().view<RigidBody, PhysicsShape, Transform>().each(
        [&](auto entt, RigidBody &rigid_body, PhysicsShape &shape, Transform &trfm) {
            RigidBodyActivity *activity;
            {
                auto result = world.scene().registry().emplace_component<RigidBodyActivity>(entt);
                activity = &result.first;

                if (result.second) {
                    // If the activity was craeted first.

                    //  * make btRigidBody

                    auto *ground_shape = make_collision_shape(shape);
                    assert(ground_shape);

                    btTransform ground_trfm;
                    ground_trfm.setIdentity();
                    //btRigidBody::btRigidBodyConstructionInfo rb_info(rigid_body.mass);
                    //auto rigid_body_backend = std::make_unique<RigidBodyBackend>(rb_info);

                    //rigid_body_backend->bind_world(*dynamics_world_);
                }

            }
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
        
        for (auto& entt : to_deleted) {
            world.scene().registry().remove_component<RigidBodyActivity>(entt);
        }
    }



    dynamics_world_->stepSimulation(world.clock().delta_time());

    // Sync with transform.
}
