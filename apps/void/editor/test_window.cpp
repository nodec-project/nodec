#include <imwindow/imwindow.hpp>
#include <game_editor/menu.hpp>

class LogWindow : imwindow::ImWindow
{
public:
    static void init()
    {

    }
};

static bool is_registered = game_editor::menu::register_menu_item("Test/Func", &LogWindow::init);
//static game_editor::menu::MenuItemRegistration _registration("Test/Func", &LogWindow::init);

