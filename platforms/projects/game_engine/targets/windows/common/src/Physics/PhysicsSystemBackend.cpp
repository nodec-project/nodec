#include <Physics/PhysicsSystemBackend.hpp>
#include <Physics/RigidBodyActivity.hpp>

#include <nodec/logging.hpp>

void PhysicsSystemBackend::on_stepped(nodec_world::World &world) {
    using namespace nodec_physics::components;

    // View RigidBody, PhysicsShape
    world.scene().registry().view<RigidBody, PhysicsShape>().each(
        [&](auto entt, RigidBody &rigidBody, PhysicsShape &shape) {
            RigidBodyActivity &activity = world.scene().registry().emplace_component<RigidBodyActivity>(entt).first;

            //  * make btRigidBody
        });

    // View RigidBodyActivity except PhysicsShape or RigidBody.

    world.scene().registry().view<RigidBodyActivity>().each([&](auto entt, RigidBodyActivity &activity) {
        //  * Finalize btRigidBody.

        if (world.scene().registry().all_of<RigidBody, PhysicsShape>(entt)) {
            return;
        }

    });

    dynamics_world_->stepSimulation(world.clock().delta_time());
}
