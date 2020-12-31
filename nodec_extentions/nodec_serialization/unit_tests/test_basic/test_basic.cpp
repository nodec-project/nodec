#include <nodec_extentions/nodec_serialization/vector2.hpp>
#include <nodec_extentions/nodec_serialization/quaternion.hpp>

#include <nodec/quaternion.hpp>

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
    std::ofstream os("out.cereal", std::ios::binary);
    cereal::JSONOutputArchive archive(os);

    SomeData myData;
    myData.data = std::make_shared<std::unordered_map<uint32_t, MyRecord>>();
    myData.data->emplace(0, MyRecord());

    auto vec = std::make_shared<Vector2f>(0, 1);
    Quaternionf quaternion;
    
    archive(myData, vec, quaternion);


    return 0;
}