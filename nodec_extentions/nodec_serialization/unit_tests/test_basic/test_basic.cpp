#include <nodec_extentions/nodec_serialization/scene_set/scene_object.hpp>
#include <nodec_extentions/nodec_serialization/scene_set/transform.hpp>
#include <nodec_extentions/nodec_serialization/vector2.hpp>
#include <nodec_extentions/nodec_serialization/quaternion.hpp>
#include <nodec_extentions/nodec_serialization/scene_set/component.hpp>

#include <nodec/scene_set/scene_object.hpp>
#include <nodec/logging.hpp>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>

#include <fstream>

using namespace nodec;

struct BaseBaseClass
{
    virtual void test() {}
};

namespace test_space
{
// A pure virtual base class
struct BaseClass : public BaseBaseClass
{
    virtual void sayType() {}

    template<class Archive>
    void serialize(Archive& ar)
    {

    }
};

}

// A class derived from BaseClass
struct DerivedClassOne : public test_space::BaseClass
{
    void sayType() {}

    int x;

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(x);
    }
};

CEREAL_REGISTER_TYPE(DerivedClassOne);

CEREAL_REGISTER_POLYMORPHIC_RELATION(test_space::BaseClass, DerivedClassOne)

class Base
{
public:
    virtual void hoge() {}

    template <class Archive>
    void serialize(Archive& archive)
    {
        //archive();
    }

};

class TestComponent : public scene_set::Component
{
public:
    std::string my_str;

public:
    using Component::Component;
    //TestComponent() :
    //    Component(nullptr)
    //{

    //}
    //void hoge() override {}
    void set_my_str(const std::string& str) { my_str = str; }

    //template <class Archive>
    //void serialize(Archive& archive)
    //{
    //    archive(my_str);
    //}

    //template <class Archive>
    //static void load_and_construct(Archive& archive, cereal::construct<TestComponent>& construct)
    //{
    //    construct();
    //    archive(construct->my_str);
    //}
};

template<class Archive>
void serialize(Archive& archive, TestComponent& test)
{
    archive(test.my_str);
}

namespace cereal
{
template<> struct LoadAndConstruct<TestComponent>
{
    template <class Archive>
    static void load_and_construct(Archive& archive, cereal::construct<TestComponent>& construct)
    {
        construct(nullptr);
        archive(construct->my_str);
    }
};
} // namespace cereal
CEREAL_REGISTER_TYPE(TestComponent);
//CEREAL_REGISTER_DYNAMIC_INIT(TestComponent);
//CEREAL_REGISTER_POLYMORPHIC_RELATION(Base, TestComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(scene_set::Component, TestComponent);

struct MyRecord
{
    uint8_t x, y;
    float z;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(x, y, z);
    }
};

struct SomeData
{
    int32_t id;
    std::shared_ptr<std::unordered_map<uint32_t, MyRecord>> data;

    template <class Archive>
    void save(Archive& ar) const
    {
        ar(data);
    }

    template <class Archive>
    void load(Archive& ar)
    {
        static int32_t idGen = 0;
        id = idGen++;
        ar(data);
    }
};



int main()
{
    logging::record_handlers += logging::StaticRecordHandler::make_shared(&logging::record_to_stdout_handler);

    try
    {
        {

            std::ofstream os("out.cereal", std::ios::binary);
            cereal::XMLOutputArchive archive(os);

            SomeData myData;
            myData.data = std::make_shared<std::unordered_map<uint32_t, MyRecord>>();
            myData.data->emplace(0, MyRecord());

            auto vec = std::make_shared<Vector2f>(0, 1);
            Quaternionf quaternion;

            //auto nodec_object = NodecObject::instanciate<NodecObject>("test");

            auto root = NodecObject::instanciate<scene_set::SceneObject>("root");
            root->append_child(NodecObject::instanciate<scene_set::SceneObject>("child_1"));
            root->append_child(NodecObject::instanciate<scene_set::SceneObject>("child_2"));
            auto child_3_ = root->append_child(NodecObject::instanciate<scene_set::SceneObject>("child_3"));

            if (auto child_3 = child_3_.lock())
            {
                child_3->append_child(NodecObject::instanciate<scene_set::SceneObject>("child_3-1"));
                child_3->transform().local_position.set(1, 3, 4);
            }
            logging::DebugStream(__FILE__, __LINE__) << root->children().size();

            //std::unordered_map<NodecObject::ID, NodecObject::Holder<scene_set::SceneObject>> children = root->children();
            //const std::unordered_map<int, int> test;

            //root->add_component<scene_set::Component>();
            //for (auto iter = test.begin(); iter != test.end(); ++iter)
            //{
            //    iter->second= 11;
            //}

            //for (auto iter = root->children().begin(); iter != root->children().end(); ++iter)
            //{
            //    iter->second = NodecObject::instanciate<scene_set::SceneObject>("child_4");
            //}

            //for (auto pair : root->children())
            //{
            //    logging::DebugStream(__FILE__, __LINE__) << pair.second->name;
            //    pair.second = NodecObject::instanciate<scene_set::SceneObject>("child_4");
            //}

            for (auto& pair : root->children())
            {
                logging::DebugStream(__FILE__, __LINE__) << pair.second->name;
                //pair.second = NodecObject::instanciate<scene_set::SceneObject>("child_4");
            }
            auto test_component = NodecObject::instanciate<TestComponent>(nullptr);

            test_component->attach_to(root.get());
            test_component->set_my_str("test");
            //std::shared_ptr<Base> base = std::make_shared<TestComponent>();
            //std::shared_ptr<test_space::BaseClass> ptr1 = std::make_shared<DerivedClassOne>();
            //std::shared_ptr<scene_set::Component> ptr1 = std::make_shared<scene_set::Transform>(nullptr);
            //std::shared_ptr<scene_set::Component> ptr1 = std::make_shared<TestComponent>();

            //archive(test_component);
            //auto transform = NodecObject::instanciate<scene_set::Transform>(nullptr);
            //archive(root, ptr1);
            //archive(ptr1);
            archive(root);
            //archive(root, transform);
            //archive(myData, vec, quaternion, nodec_object);
            //archive(vec, quaternion);
            //archive(quaternion);
            //return 0;
        }
        {
            std::ifstream is("out.cereal", std::ios::binary);
            std::ofstream os("out_2.cereal", std::ios::binary);
            cereal::XMLOutputArchive oarchive(os);

            cereal::XMLInputArchive iarchive(is);

            NodecObject::Holder<NodecObject> nodec_object;
            NodecObject::Holder<scene_set::SceneObject> root;
            std::shared_ptr<Vector2f> vec;
            Quaternionf quaternion;
            //NodecObject::Holder<scene_set::Transform> transform;

            //archive(vec, quaternion);
            //archive(root, transform);
            iarchive(root);

            for (auto& pair : root->children())
            {
                logging::DebugStream(__FILE__, __LINE__) << pair.second->name;
                //pair.second = NodecObject::instanciate<scene_set::SceneObject>("child_4");
            }
            //logging::DebugStream(__FILE__, __LINE__) << *vec << quaternion << nodec_object->name << nodec_object->id();
            oarchive(root);
        }
    }
    catch (const std::exception& e)
    {
        logging::DebugStream(__FILE__, __LINE__) << e.what();
    }


    return 0;
}