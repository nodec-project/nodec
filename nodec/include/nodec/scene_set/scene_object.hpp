#ifndef NODEC__SCENE_SET__SCENE_OBJECT_HPP_
#define NODEC__SCENE_SET__SCENE_OBJECT_HPP_

#include <nodec/scene_set/transform.hpp>
#include <nodec/scene_set/component.hpp>

#include <nodec/nodec_object.hpp>
#include <nodec/nodec_exception.hpp>

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

        public:
            template<typename T>
            NodecObject::Reference<T> add_component()
            {
                static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component.");

                if (components.find(typeid(T).hash_code()) != components.end())
                {
                    // found same type component.
                    std::ostringstream oss;
                    oss << "Component '" << typeid(T).name() << "' already exisits. Cannot add same type component.";
                    throw NodecException(oss.str(), __FILE__, __LINE__);
                }

                auto component = NodecObject::instanciate<T>(this);
                auto result = components.emplace(typeid(T).hash_code(), component);
                if (!result.second)
                {
                    std::ostringstream oss;
                    oss << "Cannot add Component '" << typeid(component).name() << "'";
                    throw NodecException(oss.str(), __FILE__, __LINE__);
                }

                component->name = typeid(T).name();

                return component;
            }

            template<typename T>
            NodecObject::Reference<T> get_component()
            {
                static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component.");

                NodecObject::Reference<T> component;
                auto iter = components.find(typeid(T).hash_code());
                if (iter != components.end())
                {
                    // Cast drom Base type "Component" to Derived type "T"
                    // <https://stackoverflow.com/questions/1358143/downcasting-shared-ptrbase-to-shared-ptrderived/14545746>
                    component = std::dynamic_pointer_cast<T>(iter->second);
                }
                return component;
            }

        private:
            std::unordered_map<size_t, NodecObject::Holder<Component>> components;

            //! pointer to transform component.
            //! transform component is always attached to the object.
            //! We don't need to worry about the life period of this component.
            Transform* p_transform;
        };

    }
}

#endif