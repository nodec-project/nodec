#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <vector>

#include <nodec_animation/resources/animation_clip.hpp>
#include <nodec_animation/animated_component_writer.hpp>
#include <nodec_scene/scene.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>
#include <nodec_serialization/nodec/vector3.hpp>

#include <cereal/types/string.hpp>
struct Resource {
    template<class Archive>
    void serialize(Archive &archive) {
    }
};

// CEREAL_REGISTER_TYPE(Resource)

struct TestComponent : nodec_scene_serialization::BaseSerializableComponent {
    TestComponent()
        : BaseSerializableComponent(this) {}

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("field", field));
        archive(cereal::make_nvp("resource", resource));
        archive(cereal::make_nvp("position", position));
    }

    float field;
    std::shared_ptr<Resource> resource;
    nodec::Vector3f position;
};

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(TestComponent)

TEST_CASE("") {
    using namespace nodec_scene_serialization;

    SceneSerialization serialization;

    serialization.register_component<TestComponent>();

    using namespace nodec_scene;
    Scene scene;

    auto root_entity = scene.create_entity("root");

    scene.registry().emplace_component<TestComponent>(root_entity);

    using namespace nodec_animation;
    using namespace nodec_animation::resources;

    AnimationClip clip;

    clip.set_curve<TestComponent>("", "field", AnimationCurve{});

    auto *animated_entity = &clip.root_entity();

    for (const auto &pair : animated_entity->components) {
        auto &component_type_info = pair.first;
        auto &component = pair.second;

        AnimatedComponentWriter writer(component_type_info, serialization);

        auto serializable_component = serialization.make_serializable_component(component_type_info);

        CHECK(serializable_component->type_info() == nodec::type_id<TestComponent>());

        // writer.sample_animation(component, serializable_component);
    }
}