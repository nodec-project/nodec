#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec_scene/scene.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>

struct TestComponent {
    int field;
};

class SerializableTestComponent : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableTestComponent()
        : BaseSerializableComponent(this) {}

    int field;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("field", field));
    }
};

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(SerializableTestComponent)

TEST_CASE("testing serialization.") {
    using namespace nodec_scene;
    using namespace nodec_scene_serialization;

    SceneSerialization serialization;

    serialization.register_component<TestComponent, SerializableTestComponent>(
        [](const TestComponent &comp) {
            auto serializable = std::make_unique<SerializableTestComponent>();
            serializable->field = comp.field;
            return serializable;
        },
        [](const SerializableTestComponent &serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &comp = registry.emplace_component<TestComponent>(entity).first;
            comp.field = serializable.field;
        });

    Scene scene;

    const auto root_entt = scene.create_entity("root");
    scene.registry().emplace_component<TestComponent>(root_entt).first.field = 100;

    auto serializable_root_entt = serialization.make_serializable_entity(root_entt, scene.registry());

    CHECK(serializable_root_entt);

    auto cloned_root_entt = serialization.make_entity(serializable_root_entt.get(), scene.registry());

    CHECK(scene.registry().get_component<TestComponent>(cloned_root_entt).field == 100);
}