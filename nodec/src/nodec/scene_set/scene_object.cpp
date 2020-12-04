#include <nodec/scene_set/scene_object.hpp>
#include <nodec/scene_set/transform.hpp>

#include <nodec/nodec_exception.hpp>

namespace nodec
{
    namespace scene_set
    {
        SceneObject::SceneObject(const std::string& name) :
            NodecObject(name),
            p_transform(nullptr)
        {
            auto tf_ref = add_component<Transform>();
            if (auto tf = tf_ref.lock())
            {
                p_transform = tf.get();
            }
            else
            {
                throw NodecException("Failed to regist transform component.", __FILE__, __LINE__);
            }
        }
        
        Transform& SceneObject::transform() const noexcept
        {
            return (*p_transform);
        }

    }
}