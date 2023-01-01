#include <nodec_scene_serialization/base_serializable_component.hpp>

class TestComponent : public nodec_scene_serialization::BaseSerializableComponent {
public:
    TestComponent()
        : BaseSerializableComponent{this} {}

    ~TestComponent() {}

    template<class Archive>
    void serialize(Archive &ar) {
        ar(x);
    }

    // void say_hello() {
    //     std::cout << "Hello";
    // }
private:
    //void rtti() {}
    int x{100};
};

CEREAL_REGISTER_TYPE(TestComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, TestComponent)


