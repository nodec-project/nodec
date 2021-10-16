#include <game_editor/impl/game_editor_module.hpp>
#include <game_editor/impl/editor_windows/control_window.hpp>

#include <imwindows/impl/log_window.hpp>
#include <imwindows/impl/hierarchy_window.hpp>

#include <nodec/logging.hpp>

using namespace nodec;

game_editor::impl::GameEditorModule::GameEditorModule() {

    logging::InfoStream(__FILE__, __LINE__)
        << "[GameEditorModule] >>> Register the editor windows.";
    imelements::register_menu_item("Window/Log",
                                   [&]() {
                                       imwindows::impl::LogWindow::init(window_manager());
                                   });
    imelements::register_menu_item("Window/Hierarchy",
                                   [&]() {
                                       imwindows::impl::HierarchyWindow::init(window_manager(), &engine_module_->scene_registry());
                                   });
    imelements::register_menu_item("Window/Control",
                                   [&]() {
                                       editor_windows::ControlWindow::init(window_manager(), this);
                                   });


    logging::InfoStream(__FILE__, __LINE__) << "[GameEditorModule] >>> Setup completed successfully.";
}
