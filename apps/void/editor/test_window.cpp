#include <imwindow/imwindow.hpp>
#include <game_editor/menu.hpp>

class LogWindow : imwindow::ImWindow
{
public:
    static void init()
    {

    }
};

class Registration
{
public:
    Registration()
    {
        game_editor::menu::register_menu_item("Test/Func", &LogWindow::init);
    }
};


Registration reg;