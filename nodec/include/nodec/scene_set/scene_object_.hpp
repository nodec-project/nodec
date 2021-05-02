#ifndef NODEC__SCENE_SET__SCENE_OBJECT_HPP_
#define NODEC__SCENE_SET__SCENE_OBJECT_HPP_

#include <nodec/scene_set/transform.hpp>
#include <nodec/scene_set/component.hpp>

#include <nodec/nodec_object.hpp>
#include <nodec/nodec_exception.hpp>
#include <nodec/logging.hpp>

#include <string>
#include <unordered_map>
#include <sstream>

#include <iostream>

namespace nodec
{
namespace scene_set
{
class SceneObject : public NodecObject
{
public:
    /**
    * @param name   object name
    */
    SceneObject(const std::string& name);

public:
    Transform& transform() const noexcept;


    NodecObject::Reference<SceneObject> parent();


    NodecObject::Reference<SceneObject> append_child(NodecObject::Holder<SceneObject> child);


    NodecObject::Holder<SceneObject> remove_child(SceneObject& child);


    const std::unordered_map<NodecObject::ID, NodecObject::Holder<SceneObject>>&
        children() const noexcept;


    const std::unordered_map<size_t, NodecObject::Holder<Component>>&
        components() const noexcept;

public:
    
    void add_component(NodecObject::Holder<Component> component)
    {
        if (&component->scene_object() != this)
        {
            std::ostringstream oss;
            oss << "Not match between the scene object of component and this scene object. \n"
                << "Being attached component must set this object as owner. see Component::attach(). \n"
                << "scene_object.name: " << name << "\n"
                << "component.name   : " << component->name;
            throw NodecException(oss.str(), __FILE__, __LINE__);
        }

        if (components_.find(typeid(*component).hash_code()) != components_.end())
        {
            // found same type component.
            std::ostringstream oss;
            oss << "Component '" << typeid(*component).name() << "' already exisits. Cannot add same type component.";
            throw NodecException(oss.str(), __FILE__, __LINE__);
        }
        auto result = components_.emplace(typeid(*component).hash_code(), component);
        if (!result.second)
        {
            std::ostringstream oss;
            oss << "Failed to add Component '" << typeid(*component).name() << "'";
            throw NodecException(oss.str(), __FILE__, __LINE__);
        }

        component->name = typeid(*component).name();
        component->on_awake();

        //nodec::logging::DebugStream(__FILE__, __LINE__) << typeid(*component).name();
    }

    template<typename T>
    NodecObject::Reference<T> add_component()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component.");

        if (components_.find(typeid(T).hash_code()) != components_.end())
        {
            // found same type component.
            std::ostringstream oss;
            oss << "Component '" << typeid(T).name() << "' already exisits. Cannot add same type component.";
            throw NodecException(oss.str(), __FILE__, __LINE__);
        }

        auto component = NodecObject::instanciate<T>(this);
        auto result = components_.emplace(typeid(T).hash_code(), component);
        if (!result.second)
        {
            std::ostringstream oss;
            oss << "Failed to add Component '" << typeid(*component).name() << "'";
            throw NodecException(oss.str(), __FILE__, __LINE__);
        }

        component->name = typeid(T).name();
        component->on_awake();

        return component;
    }

    template<typename T>
    NodecObject::Reference<T> get_component()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be inherited from Component.");

        NodecObject::Reference<T> component;
        auto iter = components_.find(typeid(T).hash_code());
        if (iter != components_.end())
        {
            // Cast from Base type "Component" to Derived type "T"
            // <https://stackoverflow.com/questions/1358143/downcasting-shared-ptrbase-to-shared-ptrderived/14545746>
            component = std::dynamic_pointer_cast<T>(iter->second);
        }
        return component;
    }

    template<typename T>
    bool remove_component()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be inherited from Component.");

        if (typeid(T) == typeid(Transform))
        {
            logging::warn("Cannot remove Transform Component. ScenenObject must have transform component.", __FILE__, __LINE__);
            return false;
        }

        return components_.erase(typeid(T).hash_code()) > 0;
    }

private:
    std::unordered_map<size_t, NodecObject::Holder<Component>> components_;

    //! pointer to transform component.
    //! transform component is always attached to the object. non-detouchable relation.
    //! We don't need to worry about the life period of this component.
    Transform* p_transform;


    //! the relation with parent and this object is detouchable.
    //! it is possible that this object still alive but parent is destroyed.
    NodecObject::Reference<SceneObject> parent_;

    std::unordered_map<NodecObject::ID, NodecObject::Holder<SceneObject>> children_;
};

}
}

#endif