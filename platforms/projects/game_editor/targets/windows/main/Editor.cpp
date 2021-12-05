#include "Editor.hpp"

#include "EditorWindows/ControlWindow.hpp"
#include "EditorWindows/AssetImportWindow.hpp"
#include "EditorWindows/SceneSerializationWindow.hpp"

#include <imwindows/scene_hierarchy_window.hpp>
#include <imwindows/entity_inspector_window.hpp>
#include <imwindows/log_window.hpp>

#include <component_gui/scene_set/basic.hpp>
#include <rendering/components/mesh_renderer.hpp>



Editor::Editor(Engine* engine)
    : engine_{ engine } {

    using namespace imwindows;
    using namespace imelements;
    using namespace scene_set::components;
    using namespace rendering::components;


    register_menu_item("Window/Control",
        [=]() { ControlWindow::init(window_manager(), this); });

    register_menu_item("Window/Entity Inspector",
        [=]() {
            EntityInspectorWindow::init(window_manager(),
                &engine->scene_module().registry(),
                &inspector_component_registry_impl(),
                selection().active_scene_entity(),
                selection().active_scene_entity_changed());
        });

    register_menu_item("Window/Log",
        [=]() {LogWindow::init(window_manager()); });

    register_menu_item("Window/Scene Hierarchy",
        [=]() {
            auto& window = SceneHierarchyWindow::init(window_manager(), &engine->scene_module());
            window.selected_entity_changed().connect([=](auto entity) {selection().select(entity); });
        });

    register_menu_item("Window/Asset Importer",
        [=]() {
            AssetImportWindow::init(window_manager(),
                engine->resources_module().resource_path(),
                &engine->scene_module(),
                &engine->resources_module().registry());
        });

    register_menu_item("Window/Scene Serialization",
        [=]() {
            SceneSerializationWindow::init(window_manager());
        });


    inspector_gui_.reset(new InspectorGUI(&engine->resources_module().registry()));

    inspector_component_registry_impl().register_component<Name>("Name", on_gui_name);
    inspector_component_registry_impl().register_component<Transform>("Transform", on_gui_transform);
    inspector_component_registry_impl().register_component<MeshRenderer>("Mesh Renderer", [=](MeshRenderer& renderer) {
        inspector_gui_->OnGUIMeshRenderer(renderer);
        });


    //register_menu_item("Test", [=]() {
    //    using namespace nodec;
    //    using namespace rendering::resources;

    //    std::ofstream out(Formatter() << engine_->resources_module().resource_path() << "/shader.meta", std::ios::binary);

    //    if (!out) {
    //        logging::WarnStream(__FILE__, __LINE__) << "Failed";
    //        return;
    //    }

    //    cereal::JSONOutputArchive archive(out);

    //    SerializableShaderMetaInfo meta;
    //    meta.float_properties.push_back({ "float_a", 0.0f });
    //    meta.float_properties.push_back({ "float_b", 1.0f });

    //    meta.vector4_properties.push_back({ "vector_a", {0.0f, 0.0f, 0.0f, 0.0f} });
    //    meta.vector4_properties.push_back({ "vector_b", {1.0f, 1.0f, 1.0f, 1.0f} });

    //    meta.texture_entries.push_back({ "texture_a" });
    //    meta.texture_entries.push_back({ "texture_b" });

    //    archive(cereal::make_nvp("meta", meta));
    //    });
}