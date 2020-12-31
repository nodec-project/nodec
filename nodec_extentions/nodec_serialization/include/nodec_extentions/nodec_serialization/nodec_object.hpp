#ifndef NODEC_EXTENTIONS__NODEC_SERIALIZATION__NODEC_OBJECT_HPP_
#define NODEC_EXTENTIONS__NODEC_SERIALIZATION__NODEC_OBJECT_HPP_

#include <nodec/nodec_object.hpp>
#include <cereal/access.hpp>

namespace nodec
{

template<class Archive>
void serialize(Archive& archive, NodecObject& obj)
{
    archive(obj.name);
}

}
namespace cereal
{
template <> struct LoadAndConstruct<nodec::NodecObject>
{
    template <class Archive>
    static void load_and_construct(Archive& ar, cereal::construct<nodec::NodecObject>& construct)
    {
        std::string name;
        ar(name);
        construct(name);
    }
};
}

#endif