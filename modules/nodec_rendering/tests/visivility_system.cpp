#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec_rendering/systems/visibility_system.hpp>
#include <nodec_scene/scene.hpp>

TEST_CASE("Testing the addition and removal of NonVisible components.") {
    using namespace nodec_scene;
    using namespace nodec_rendering::systems;
    using namespace nodec_rendering::components;

    Scene scene;

    VisibilitySystem visibility_system{&scene.registry()};

    auto root = scene.create_entity();
    auto child_1 = scene.create_entity();
    auto child_1_1 = scene.create_entity();
    auto child_1_2 = scene.create_entity();
    auto child_2 = scene.create_entity();
    scene.hierarchy_system().append_child(root, child_1);
    scene.hierarchy_system().append_child(child_1, child_1_1);
    scene.hierarchy_system().append_child(child_1, child_1_2);
    scene.hierarchy_system().append_child(root, child_2);

    SUBCASE("Testing simple case.") {
        {
            auto &non_visible = scene.registry().emplace_component<NonVisible>(child_1, true).first;
            CHECK(non_visible.self);
        }

        CHECK(scene.registry().try_get_component<NonVisible>(child_1) != nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_1) != nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_2) != nullptr);

        scene.registry().remove_component<NonVisible>(child_1);

        CHECK(scene.registry().try_get_component<NonVisible>(child_1) == nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_1) == nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_2) == nullptr);
    }

    SUBCASE("Testing removal of component with child having self flag.") {
        scene.registry().emplace_component<NonVisible>(root, true);
        scene.registry().get_component<NonVisible>(child_1).self = true;

        CHECK(scene.registry().remove_component<NonVisible>(root));

        CHECK(scene.registry().try_get_component<NonVisible>(child_1) != nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_1) != nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_2) != nullptr);

        CHECK(scene.registry().remove_component<NonVisible>(child_1));

        CHECK(scene.registry().try_get_component<NonVisible>(child_1) == nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_1) == nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_2) == nullptr);
    }
}