#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec_scene_serialization/entity_emplacer.hpp>

#include <memory>

TEST_CASE("testing emplacement.") {
    using namespace nodec::entities;
    using namespace nodec_scene;
    using namespace nodec_scene::components;
    using namespace nodec_scene_serialization;

    SceneSerialization serialization;
    auto root = std::make_unique<SerializableEntity>();

    root->children.emplace_back(std::make_unique<SerializableEntity>());
    root->children.emplace_back(std::make_unique<SerializableEntity>());

    Scene scene;

    auto root_entity = EntityEmplacer(serialization).emplace(root.get(), null_entity, scene);
    CHECK(scene.registry().is_valid(root_entity));

    CHECK(scene.registry().get_component<Hierarchy>(root_entity).child_count == 2);
}