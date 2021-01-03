#ifndef NODEC_EXTENTIONS__NODEC_SERIALIZATION__SCENE_SET__COMPONENT_HPP_
#define NODEC_EXTENTIONS__NODEC_SERIALIZATION__SCENE_SET__COMPONENT_HPP_

#include <nodec/scene_set/component.hpp>

#include <cereal/types/polymorphic.hpp>


namespace nodec
{
namespace scene_set
{

template<class Archive>
void serialize(Archive& archive, Component& component)
{
    //archive(component.name);
}

} // namespace scene_set
} // namespace nodec

namespace cereal
{
template<> struct LoadAndConstruct<nodec::scene_set::Component>
{
    template <class Archive>
    static void load_and_construct(Archive& archive, cereal::construct<nodec::scene_set::Component>& construct)
    {
        //std::string name;
        //archive(name);
        construct(nullptr);
    }
};
} // namespace cereal

//#include <cereal/archives/binary.hpp>
//#include <cereal/archives/xml.hpp>
//#include <cereal/archives/json.hpp>
//CEREAL_REGISTER_TYPE(nodec::scene_set::Component);

#endif