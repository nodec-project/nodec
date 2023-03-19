#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec_scene_serialization/entity_serializer.hpp>

#include <cereal/cereal.hpp>

TEST_CASE("testing serialization.") {
    using namespace nodec_scene;
    using namespace nodec_scene_serialization;

    SceneSerialization serialization;

    Scene scene;

    auto root = scene.create_entity("root");
    {
        auto child = scene.create_entity("child1");
        scene.hierarchy_system().append_child(root, child);
    }
    {
        auto child = scene.create_entity("child2");
        scene.hierarchy_system().append_child(root, child);
    }

    auto serializable_root = EntitySerializer(root, scene, serialization).serialize();
    CHECK(serializable_root);

    CHECK(serializable_root->children.size() == 2);
}