#include "Editor.hpp"

#include "EditorWindows/ControlWindow.hpp"

#include <imwindows/scene_hierarchy_window.hpp>
#include <imwindows/inspector_window.hpp>
#include <imwindows/log_window.hpp>



Editor::Editor(Engine* engine) 
    : engine_{ engine } 
{

    using namespace imwindows;

    imelements::register_menu_item("Window/Log",
                                   [=]() {LogWindow::init(window_manager()); });

    imelements::register_menu_item("Window/Hierarchy",
                                   [=]() {
                                       SceneHierarchyWindow::init(window_manager(), &engine->scene_module());
                                   });

    imelements::register_menu_item("Window/Control",
                                   [=]() { ControlWindow::init(window_manager(), this); });



    

}