
#define CEREAL_THREAD_SAFE 1

#include "Editor.hpp"

#include "EditorWindows/ControlWindow.hpp"
#include "EditorWindows/AssetImportWindow.hpp"
#include "EditorWindows/SceneSerializationWindow.hpp"

#include <imwindows/scene_hierarchy_window.hpp>
#include <imwindows/entity_inspector_window.hpp>
#include <imwindows/log_window.hpp>



Editor::Editor(Engine* engine)
    : engine_{ engine } {

    using namespace imwindows;
    using namespace imelements;
    using namespace scene_set::components;
    using namespace rendering::components;
    using namespace scene_audio::components;


    register_menu_item("Window/Control",
        [=]() { ControlWindow::init(window_manager(), this); });

    register_menu_item("Window/Entity Inspector",
        [=]() {
            EntityInspectorWindow::init(
                window_manager(),
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
            AssetImportWindow::init(
                window_manager(),
                engine->resources_module().resource_path(),
                &engine->scene_module(),
                &engine->resources_module().registry());
        });

    register_menu_item("Window/Scene Serialization",
        [=]() {
            SceneSerializationWindow::init(
                window_manager(),
                &engine->scene_module(),
                &engine->scene_serialization(),
                engine->resources_module().resource_path(),
                &engine->resources_module().registry(),
                selection().active_scene_entity(),
                selection().active_scene_entity_changed()
            );
        });


    inspector_gui_.reset(new InspectorGUI(&engine->resources_module().registry()));

    inspector_component_registry_impl().register_component<Name>(
        "Name",
        [=](auto& name) {
            inspector_gui_->OnGUIName(name);
        });

    inspector_component_registry_impl().register_component<Transform>(
        "Transform",
        [=](auto& trfm) {
            inspector_gui_->onGUITransform(trfm);
        });

    inspector_component_registry_impl().register_component<MeshRenderer>(
        "Mesh Renderer",
        [=](auto& renderer) {
            inspector_gui_->OnGUIMeshRenderer(renderer);
        });

    inspector_component_registry_impl().register_component<Camera>(
        "Camera",
        [=](auto& camera) {
            inspector_gui_->OnGUICamera(camera);
        });

    inspector_component_registry_impl().register_component<Light>(
        "Light",
        [=](auto& light) {
            inspector_gui_->OnGUILight(light);
        });

    inspector_component_registry_impl().register_component<AudioSource>(
        "AudioSource",
        [=](auto& source) {

        });


}