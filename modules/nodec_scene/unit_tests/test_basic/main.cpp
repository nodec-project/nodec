
#include <nodec_scene/impl/scene_module.hpp>
#include <nodec_scene/components/standard.hpp>
#include <nodec_scene/systems/hierarchy_system.hpp>
#include <nodec_scene/systems/impl/impl_hierarchy_system.hpp>

#include <nodec/logging.hpp>
#include <nodec/formatter.hpp>
#include <nodec/error_formatter.hpp>

#include <iostream>


using namespace nodec;
using namespace nodec_scene;

void print_hierarchy(SceneRegistry& registry) {
    auto view = registry.view<components::Hierarchy>();

    for (auto entity : view) {
        auto& hier = registry.get_component<const components::Hierarchy>(entity);
        logging::InfoStream info(__FILE__, __LINE__);
        info << "\n" << entity << ": \n";
        info << "  parent  : " << hier.parent << "\n";
        info << "  children: ";
        for (auto child : hier.children) {
            info << child << ", ";
        }
    }
}

int main() {
    logging::record_handlers().connect(logging::record_to_stdout_handler);
    //logging::record_handlers += event::StaticCallback<const logging::LogRecord&>::make_shared(&logging::record_to_stdout_handler);
    logging::info("Start", __FILE__, __LINE__);


    try {
        nodec_scene::impl::SceneModule scene_module;

        Scene& scene = scene_module;

        auto entity_root = scene.registry().create_entity();
        auto entity_child_1 = scene.registry().create_entity();
        auto entity_child_1_1 = scene.registry().create_entity();

        nodec_scene::systems::append_child(scene.registry(), entity_root, entity_child_1);
        nodec_scene::systems::append_child(scene.registry(), entity_child_1, entity_root);


        //nodec_scene::systems::append_child(scene.registry())

        


        //SceneRegistry registry;
        //systems::impl::HierarchySystem hierarchySystem;

        //{
        //    hierarchySystem.init(registry);
        //}

        //auto entity_root = registry.create_entity();
        //{
        //    registry.emplace_component<components::Hierarchy>(entity_root);
        //    //auto pair = registry.emplace_component<components::Hierarchy>(entity_root);
        //    //logging::InfoStream(__FILE__, __LINE__) << "added hierarch?: " << pair.second;

        //    registry.emplace_component<components::Name>(entity_root, "ROOT");
        //    registry.emplace_component<components::Transform>(entity_root);
        //}


        //{
        //    auto entity_child_a = registry.create_entity();
        //    registry.emplace_component<components::Hierarchy>(entity_child_a);
        //    registry.emplace_component<components::Transform>(entity_child_a);


        //    auto entity_child_a_a = registry.create_entity();
        //    registry.emplace_component<components::Transform>(entity_child_a_a);
        //    //registry.emplace_component<components::Hierarchy>(entity_child_a_a);

        //    {
        //        //components::Hierarchy::append_child(registry, entity_child_a, entity_child_a);
        //        systems::append_child(registry, entity_root, entity_child_a);
        //        systems::append_child(registry, entity_child_a, entity_child_a_a);
        //        //systems::remove_child(registry, entity_root, entity_child_a);
        //        //systems::append_child(registry, entity_child_a_a, entity_root);
        //    }
        //    {
        //        auto entity = registry.create_entity();
        //        systems::append_child(registry, entity_child_a_a, entity);
        //        systems::append_child(registry, entity_root, entity);
        //    }
        //    print_hierarchy(registry);

        //    logging::info("Destroy", __FILE__, __LINE__);
        //    registry.destroy_entity(entity_child_a);
        //}

        //

        //{
        //    print_hierarchy(registry);
        //}

        //logging::info(Formatter() << "test", __FILE__, __LINE__);

        //throw std::runtime_error(error_fomatter::type_file_line<std::runtime_error>(
        //    Formatter() << "This is the test " << 1 << ". Opps!", __FILE__, __LINE__));

        //Matrix4x4d mat{
        //    0, 1, 2, 3,
        //    4, 5, 6, 7,
        //    8, 9, 10, 11,
        //    12, 13, 14, 15
        //};

        //Matrix4x4d mat1{
        //    1, 2, 3,
        //    4, 5, 6, 7,
        //    8, 9, 10, 11,
        //    12, 13, 14, 15, 16
        //};
        ////logging::InfoStream(__FILE__, __LINE__) << static_cast<Matrix4x4f>(mat) * Matrix4x4f::identity;
        //logging::InfoStream(__FILE__, __LINE__) << mat * mat1;
        //logging::InfoStream(__FILE__, __LINE__) << math::gfx::trs({ 0, 1, 2 }, Quaternionf::identity, { 5, 6, 7 });
        ////Vector3f vec;
        ////auto vec = static_cast<Vector3d>(vec) * Vector3d::ones;

        //auto trfm = components::Transform();
        //trfm.set_local_position(trfm.local_position() + Vector3f(1, 0, 0));
        //logging::InfoStream(__FILE__, __LINE__) << trfm.local_position();

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