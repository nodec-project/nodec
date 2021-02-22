#include <game_editor/impl/menu_impl.hpp>

#include <imgui.h>

#include <string>
#include <functional>
#include <map>


namespace game_editor
{
namespace menu
{

namespace impl
{

class MenuItem
{
public:
    MenuItem(const std::string& name, std::function<void()> func, int order) :
        name(name),
        func(func),
        order(order)
    {
    }

    std::string name;
    std::function<void()> func;
    int order;

    std::multimap<int, MenuItem> items;
};


void show_main_menu()
{

}

void set_menu_item_order(int order)
{

}

}

void register_menu_item(const std::string& item_name, std::function<void()> func)
{

}


}
}