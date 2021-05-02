
#include <nodec/scene_set/components/standard.hpp>

#include <nodec/logging.hpp>

#include <iostream>


using namespace nodec;
using namespace nodec::scene_set;

int main() {

    logging::record_handlers += event::StaticCallback<const logging::LogRecord&>::make_shared(&logging::record_to_stdout_handler);
    logging::info("Start", __FILE__, __LINE__);

    SceneRegistry registry;

    try {

        Matrix4x4d mat{
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10, 11,
            12, 13, 14, 15
        };

        Matrix4x4d mat1{
            1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10, 11,
            12, 13, 14, 15, 16
        };
        //logging::InfoStream(__FILE__, __LINE__) << static_cast<Matrix4x4f>(mat) * Matrix4x4f::identity;
        logging::InfoStream(__FILE__, __LINE__) << mat * mat1;
        
        //Vector3f vec;
        //auto vec = static_cast<Vector3d>(vec) * Vector3d::ones;

        auto entity_root = registry.create_entity();

        {
            auto& com = registry.add_component<components::Hierarchy>(entity_root);
        }
        {
            auto& com = registry.add_component<components::Name>(entity_root, "ROOT");
        }

        auto entity_child_a = registry.create_entity();
        registry.add_component<components::Hierarchy>(entity_child_a);

        auto entity_child_a_a = registry.create_entity();
        registry.add_component<components::Hierarchy>(entity_child_a_a);

        
        {
            //components::Hierarchy::append_child(registry, entity_child_a, entity_child_a);
            components::Hierarchy::append_child(registry, entity_root, entity_child_a);
            components::Hierarchy::append_child(registry, entity_child_a, entity_child_a_a);
            components::Hierarchy::remove_child(registry, entity_root, entity_child_a);
        }

        //std::cout << "--- 1 ---" << std::endl;
        //auto root_object = NodecObject::instanciate<SceneObject>("root");

        //std::cout << root_object->id() << std::endl;
        //std::cout << root_object->name << std::endl;

        //{
        //    {
        //        //auto wrong_used_component = NodecObject::instanciate<Component>(); // <- ERROR
        //        //std::cout << wrong_used_component->name << std::endl;
        //    }
        //    auto component = root_object->add_component<Component>();
        //    //auto behavior = root_object->add_component<Behavior>();
        //    //auto component_ = root_object->add_component<Component>();
        //    
        //    if (auto com = component.lock())
        //    {
        //        std::cout << com->name << std::endl;
        //        std::cout << com->id() << std::endl;
        //    }
        //    
        //}

        //std::cout << "--- 2 ---" << std::endl;
        //std::cout << root_object->transform().name << std::endl;
        //std::cout << root_object->transform().id() << std::endl;

        //logging::DebugStream(__FILE__, __LINE__) << root_object->remove_component<Transform>();

        ////auto transform_ref = root_object->get_component<Behavior>();
        ////if (auto transform = transform_ref.lock())
        ////{
        ////    std::cout << transform->name << std::endl;
        ////    std::cout << transform->id() << std::endl;
        ////}

        //logging::debug("--- 3 ---", __FILE__, __LINE__);
        //{
        //    auto child_1_ref = root_object->append_child(NodecObject::instanciate<SceneObject>("child_1"));
        //    if (auto child_1 = child_1_ref.lock())
        //    {
        //        logging::DebugStream(__FILE__, __LINE__) << child_1->name << std::flush;
        //        logging::DebugStream(__FILE__, __LINE__) << child_1->parent().lock()->name << std::flush;
        //    }

        //    auto child_2 = NodecObject::instanciate<SceneObject>("child_2");
        //    //root_object->remove_child(*child_2); // Error
        //    if (auto child_2 = child_1_ref.lock())
        //    {
        //        auto removed = root_object->remove_child(*child_2);
        //        logging::DebugStream(__FILE__, __LINE__) << removed->name << std::flush;
        //    }
        //    //throw std::runtime_error("test");
        //}

        //logging::debug("--- 4 ---", __FILE__, __LINE__);
        //{
        //    auto root = NodecObject::instanciate<SceneObject>("root");
        //    auto child_1 = NodecObject::instanciate<SceneObject>("child_1");
        //    root->append_child(child_1);
        //    child_1->transform().local_position.set(1, 2, 3);
        //    auto child_1_1 = NodecObject::instanciate<SceneObject>("child_1_");
        //    child_1->append_child(child_1_1);
        //    child_1_1->transform().local_position.set(4, 5, 6);
        //    logging::DebugStream(__FILE__, __LINE__) << child_1_1->transform().get_world_position() << std::flush;
        //    root->transform().local_position.set(-4, -3, -2);
        //    logging::DebugStream(__FILE__, __LINE__) << child_1_1->transform().get_world_position() << std::flush;
        //}
    }
    catch (const std::exception& e) {
        logging::FatalStream(__FILE__, __LINE__) << e.what();
    }
    return 0;
}