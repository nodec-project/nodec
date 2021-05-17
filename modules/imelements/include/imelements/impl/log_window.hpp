#ifndef IMELEMENTS__IMPL__LOG_WINDOW_HPP_
#define IMELEMENTS__IMPL__LOG_WINDOW_HPP_

#include <imelements/window.hpp>

#include <nodec/vector2.hpp>
#include <nodec/logging.hpp>

namespace imelements {
namespace impl {

class LogWindow : public BaseWindow {
public:
    static void init(WindowManager& manager) {
        auto& window = manager.get_window<LogWindow>();

        nodec::logging::info("test", __FILE__, __LINE__);
    }

public:
    LogWindow()
        : BaseWindow("Log", nodec::Vector2f(500, 400)) {
    };
};


//
//inline static bool registered = game_editor::menu::register_menu_item("Window/Log", LogWindow::init);

}
}

#endif