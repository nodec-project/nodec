#ifndef NODEC_EXTENTIONS__NODEC_SERIALIZATION__SCENE_SET__SCENE_OBJECT_HPP_
#define NODEC_EXTENTIONS__NODEC_SERIALIZATION__SCENE_SET__SCENE_OBJECT_HPP_

#include <nodec/scene_set/scene_object.hpp>

#include <cereal/access.hpp>
#include <cereal/types/vector.hpp>

#include <vector>

namespace nodec
{
namespace scene_set
{

template<class Archive>
void serialize(Archive& archive, SceneObject& scene_obj)
{
    std::vector<NodecObject::Holder<SceneObject>> children;
    for (auto& pair : scene_obj.children())
    {
        children.push_back(pair.second);
    }

    std::vector<NodecObject::Holder<Component>> components;
    for (auto& pair : scene_obj.components())
    {
        components.push_back(pair.second);
    }

    archive(scene_obj.name, children);
    //archive(scene_obj.name, children, components);
}


} // namespace scene_set
} // nodec


namespace cereal
{
template <> struct LoadAndConstruct<nodec::scene_set::SceneObject>
{
    template <class Archive>
    static void load_and_construct(Archive& archive, cereal::construct<nodec::scene_set::SceneObject>& construct)
    {
        std::string name;
        std::vector<nodec::NodecObject::Holder<nodec::scene_set::SceneObject>> children;
        std::vector<nodec::NodecObject::Holder<nodec::scene_set::Component>> components;

        archive(name, children);
        //archive(name, children, components);
        construct(name);

        for (auto& child : children)
        {
            construct->append_child(child);
        }

    }
};
} // namespace cereal


#endif