#include <nodec_extentions/nodec_serialization/vector2.hpp>
#include <nodec_extentions/nodec_serialization/quaternion.hpp>
#include <nodec_extentions/nodec_serialization/scene_set/scene_object.hpp>

#include <nodec/scene_set/scene_object.hpp>
#include <nodec/logging.hpp>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <fstream>


using namespace nodec;

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

    {
        std::ofstream os("out.cereal", std::ios::binary);
        cereal::JSONOutputArchive archive(os);

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
        }
        logging::DebugStream(__FILE__, __LINE__) << root->children().size();

        //std::unordered_map<NodecObject::ID, NodecObject::Holder<scene_set::SceneObject>> children = root->children();
        const std::unordered_map<int, int> test;

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

        archive(root);
        //archive(myData, vec, quaternion, nodec_object);
        //archive(vec, quaternion);
        //archive(quaternion);
    }
    {
        std::ifstream is("out.cereal", std::ios::binary);
        cereal::JSONInputArchive archive(is);

        NodecObject::Holder<NodecObject> nodec_object;
        NodecObject::Holder<scene_set::SceneObject> root;
        std::shared_ptr<Vector2f> vec;
        Quaternionf quaternion;

        //archive(vec, quaternion);
        archive(root);

        for (auto& pair : root->children())
        {
            logging::DebugStream(__FILE__, __LINE__) << pair.second->name;
            //pair.second = NodecObject::instanciate<scene_set::SceneObject>("child_4");
        }

        //logging::DebugStream(__FILE__, __LINE__) << *vec << quaternion << nodec_object->name << nodec_object->id();

    }

    return 0;
}