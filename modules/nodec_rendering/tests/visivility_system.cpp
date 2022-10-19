#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec_rendering/systems/visibility_system.hpp>
#include <nodec_scene/scene.hpp>

TEST_CASE("Testing set_visible().") {
    using namespace nodec_scene;
    using namespace nodec_rendering::systems;
    using namespace nodec_rendering::components;

    Scene scene;

    VisibilitySystem visibility_system{scene};

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
        set_visible(scene.registry(), child_1, false);

        CHECK(scene.registry().try_get_component<NonVisible>(child_1) != nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_1) != nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_2) != nullptr);

        CHECK(scene.registry().get_component<NonVisible>(child_1).self);

        set_visible(scene.registry(), child_1, true);

        CHECK(scene.registry().try_get_component<NonVisible>(child_1) == nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_1) == nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_2) == nullptr);
    }

    SUBCASE("Testing removal of component with child having self flag.") {
        set_visible(scene.registry(), root, false);
        set_visible(scene.registry(), child_1, false);

        set_visible(scene.registry(), root, true);

        CHECK(scene.registry().try_get_component<NonVisible>(child_1) != nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_1) != nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_2) != nullptr);

        set_visible(scene.registry(), child_1, true);

        CHECK(scene.registry().try_get_component<NonVisible>(child_1) == nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_1) == nullptr);
        CHECK(scene.registry().try_get_component<NonVisible>(child_1_2) == nullptr);
    }
}