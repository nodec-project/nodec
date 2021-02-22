#include <imwindow/imwindow.hpp>
#include <game_editor/menu.hpp>

class LogWindow : imwindow::ImWindow
{
public:
    static void init()
    {

    }
};

static bool is_registered_0 = game_editor::menu::register_menu_item("Window/Log", &LogWindow::init);
static bool is_registered_1 = game_editor::menu::register_menu_item("File/Event", &LogWindow::init);
static bool is_registered_2 = game_editor::menu::register_menu_item("File/Event/A", &LogWindow::init);
static bool is_registered_3 = game_editor::menu::register_menu_item("File/Event/B", &LogWindow::init);
//static game_editor::menu::MenuItemRegistration _registration("Test/Func", &LogWindow::init);

