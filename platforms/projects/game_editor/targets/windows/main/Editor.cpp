#include "Editor.hpp"

#include "EditorWindows/ControlWindow.hpp"

#include <imwindows/scene_hierarchy_window.hpp>
#include <imwindows/inspector_window.hpp>
#include <imwindows/log_window.hpp>



Editor::Editor(Engine* engine)
    : engine_{ engine } {

    using namespace imwindows;
    using namespace imelements;

    register_menu_item("Window/Control",
                       [=]() { ControlWindow::init(window_manager(), this); });

    register_menu_item("Window/Inspector",
                       [=]() {
                           InspectorWindow::init(window_manager(), &engine->scene_module().registry(), selection().active_scene_entity_changed());
                       });

    register_menu_item("Window/Log",
                       [=]() {LogWindow::init(window_manager()); });

    register_menu_item("Window/Hierarchy",
                       [=]() {
                           auto& window = SceneHierarchyWindow::init(window_manager(), &engine->scene_module());
                           window.selected_entity_changed().connect([=](auto entity) {selection().select(entity); });
                       });






}