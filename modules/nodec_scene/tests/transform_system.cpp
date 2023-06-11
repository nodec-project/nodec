#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec_scene/scene.hpp>
#include <nodec_scene/systems/transform_system.hpp>

TEST_CASE("Testing update_transform()") {
    using namespace nodec_scene;
    using namespace nodec_scene::components;
    using namespace nodec_scene::systems;
    using namespace nodec;

    {
        Scene scene;

        auto root = scene.create_entity();
        auto child_1 = scene.create_entity();

        scene.hierarchy_system().append_child(root, child_1);

        {
            auto &root_local_trfm = scene.registry().emplace_component<LocalTransform>(root).first;
            root_local_trfm.position.set(1.f, 1.f, 1.f);
            root_local_trfm.dirty = true;
        }

        update_transform(scene.registry(), root);

        const Matrix4x4f expected(
            1.f, 0.f, 0.f, 1.f,
            0.f, 1.f, 0.f, 1.f,
            0.f, 0.f, 1.f, 1.f,
            0.f, 0.f, 0.f, 1.f);

        CHECK_EQ(scene.registry().get_component<LocalToWorld>(root).value, expected);
        CHECK_EQ(scene.registry().get_component<LocalToWorld>(child_1).value, expected);
    }
}