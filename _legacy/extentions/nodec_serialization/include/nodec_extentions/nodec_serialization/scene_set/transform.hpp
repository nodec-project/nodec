#ifndef NODEC_EXTENTIONS__NODEC_SERIALIZATION__SCENE_SET__TRANSFORM_HPP_
#define NODEC_EXTENTIONS__NODEC_SERIALIZATION__SCENE_SET__TRANSFORM_HPP_

#include <nodec_extentions/nodec_serialization/vector3.hpp>
#include <nodec_extentions/nodec_serialization/quaternion.hpp>

#include <nodec/scene_set/transform.hpp>

#include <cereal/access.hpp>
#include <cereal/types/polymorphic.hpp>


namespace nodec
{
namespace scene_set
{

template<class Archive>
void serialize(Archive& archive, Transform& transform)
{
    archive(transform.local_position, transform.local_rotation, transform.local_scale);
}

} // namespace scene_set
} // namespace nodec


namespace cereal
{
template<> struct LoadAndConstruct<nodec::scene_set::Transform>
{
    template <class Archive>
    static void load_and_construct(Archive& archive, cereal::construct<nodec::scene_set::Transform>& construct)
    {
        nodec::Vector3f local_position, local_scale;
        nodec::Quaternionf local_rotation;
        archive(local_position, local_rotation, local_scale);

        construct(nullptr);
        construct->local_position = local_position;
        construct->local_rotation = local_rotation;
        construct->local_scale = local_scale;
    }
};
} // namespace cereal


#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
CEREAL_REGISTER_TYPE(nodec::scene_set::Transform);

CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec::scene_set::Component, nodec::scene_set::Transform);




#endif