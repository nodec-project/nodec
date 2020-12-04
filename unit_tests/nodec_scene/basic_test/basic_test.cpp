#include <nodec/scene_set/scene_object.hpp>
#include <nodec/scene_set/behavior.hpp>

#include <iostream>

using namespace nodec;
using namespace nodec::scene_set;

int main()
{
    try
    {
        std::cout << "--- 1 ---" << std::endl;
        auto root_object = NodecObject::instanciate<SceneObject>("root");

        std::cout << root_object->id() << std::endl;
        std::cout << root_object->name << std::endl;

        {
            {
                //auto wrong_used_component = NodecObject::instanciate<Component>(); // <- ERROR
                //std::cout << wrong_used_component->name << std::endl;
            }
            auto component = root_object->add_component<Component>();
            auto behavior = root_object->add_component<Behavior>();
            //auto component_ = root_object->add_component<Component>();
            
            if (auto com = component.lock())
            {
                std::cout << com->name << std::endl;
                std::cout << com->id() << std::endl;
            }
        }

        std::cout << "--- 2 ---" << std::endl;
        std::cout << root_object->transform().name << std::endl;
        std::cout << root_object->transform().id() << std::endl;

        auto transform_ref = root_object->get_component<Behavior>();
        if (auto transform = transform_ref.lock())
        {
            std::cout << transform->name << std::endl;
            std::cout << transform->id() << std::endl;
        }
    }
    catch (const NodecException& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}