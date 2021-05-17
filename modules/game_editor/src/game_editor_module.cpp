#include <game_editor/impl/game_editor_module.hpp>
#include <imelements/impl/log_window.hpp>

#include <nodec/logging.hpp>

using namespace nodec;

game_editor::impl::GameEditorModule::GameEditorModule() {
    logging::InfoStream(__FILE__, __LINE__) << "[GameEditorModule] >>> Start the setup...";
    
    logging::InfoStream(__FILE__, __LINE__) << "[GameEditorModule] >>> Register the editor windows.";
    imelements::register_menu_item("Window/Log",
                                   [&]() {
                                       imelements::impl::LogWindow::init(window_manager());
                                   });


    logging::InfoStream(__FILE__, __LINE__) << "[GameEditorModule] >>> Setup completed successfully.";
}
