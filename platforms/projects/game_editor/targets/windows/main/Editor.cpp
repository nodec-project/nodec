#include "Editor.hpp"

#include "EditorWindows/ControlWindow.hpp"
#include "EditorWindows/ResourceImportWindow.hpp"

#include <imwindows/scene_hierarchy_window.hpp>
#include <imwindows/entity_inspector_window.hpp>
#include <imwindows/log_window.hpp>

#include <component_gui/scene_set/basic.hpp>


Editor::Editor(Engine* engine)
    : engine_{ engine } {

    using namespace imwindows;
    using namespace imelements;
    using namespace scene_set::components;

    register_menu_item("Window/Control",
                       [=]() { ControlWindow::init(window_manager(), this); });

    register_menu_item("Window/Entity Inspector",
                       [=]() {
                           EntityInspectorWindow::init(window_manager(), 
                                                       &engine->scene_module().registry(),
                                                       &inspector_component_registry_impl(),
                                                       selection().active_scene_entity_changed());
                       });

    register_menu_item("Window/Log",
                       [=]() {LogWindow::init(window_manager()); });

    register_menu_item("Window/Scene Hierarchy",
                       [=]() {
                           auto& window = SceneHierarchyWindow::init(window_manager(), &engine->scene_module());
                           window.selected_entity_changed().connect([=](auto entity) {selection().select(entity); });
                       });

    register_menu_item("Window/Resource Importer",
                       [=]() {
                           ResourceImportWindow::init(window_manager(), engine->resources_module().resource_path());
                       });




    inspector_component_registry_impl().register_component<Name>("Name", on_gui_name);
    inspector_component_registry_impl().register_component<Transform>("Transform", on_gui_transform);



}