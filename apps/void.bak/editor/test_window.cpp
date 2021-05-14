#include <imwindow/imwindow.hpp>
#include <game_editor/menu.hpp>


#include <nodec/vector2.hpp>
#include <nodec/logging.hpp>


class LogWindow : public imwindow::ImWindow
{
public:

    static void init()
    {
        auto window = imwindow::get_window<LogWindow>();

        nodec::logging::info("test", __FILE__, __LINE__);
    }

public:
    LogWindow() :
        ImWindow("Log", nodec::Vector2f(500, 400))
    {
    };
};

static bool is_registered_0 = game_editor::menu::register_menu_item("Window/Log", &LogWindow::init);

//static game_editor::menu::MenuItemRegistration _registration("Test/Func", &LogWindow::init);

