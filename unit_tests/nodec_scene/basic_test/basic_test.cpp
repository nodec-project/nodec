#include <nodec/scene_set/scene_object.hpp>
#include <nodec/scene_set/behavior.hpp>

#include <nodec/logging.hpp>

#include <iostream>

using namespace nodec;
using namespace nodec::scene_set;

int main()
{

    logging::record_handlers += event::StaticCallback<const logging::LogRecord&>::make_shared(&logging::record_to_stdout_handler);
    logging::info("log start", __FILE__, __LINE__);

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

        logging::DebugStream(__FILE__, __LINE__) << root_object->remove_component<Transform>();

        auto transform_ref = root_object->get_component<Behavior>();
        if (auto transform = transform_ref.lock())
        {
            std::cout << transform->name << std::endl;
            std::cout << transform->id() << std::endl;
        }

        logging::debug("--- 3 ---", __FILE__, __LINE__);
        {
            
        }
    }
    catch (const NodecException& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}