#include <game_editor/impl/menu_impl.hpp>

#include <imgui.h>

#include <nodec/macros.hpp>

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
    NODEC_SMART_PTR_DEFINITIONS(MenuItem);
    
    MenuItem(const std::string& name, std::function<void()> func, int order) :
        name(name),
        func(func),
        order(order)
    {
    }

    std::string name;
    std::function<void()> func;
    int order;

    std::multimap<int, MenuItem::SharedPtr> items;
};

static MenuItem::UniquePtr root_menu_item;

static void init()
{
    if (!impl::root_menu_item)
    {
        impl::root_menu_item = impl::MenuItem::make_unique("root", nullptr, 0);
    }
}

static void show_menu_items()
{

}

void show_main_menu()
{
    init();

    if (ImGui::BeginMainMenuBar())
    {
        for (auto& pair : root_menu_item->items)
        {
            if (ImGui::BeginMenu(pair.second->name.c_str()))
            {
                ImGui::EndMenu();
            }
            //pair.second->name;

            //item.second.func();
        }

        //if (ImGui::BeginMenu("Window"))
        //{

        //    ImGui::EndMenu();
        //}
        ImGui::EndMainMenuBar();
    }
}

void set_menu_item_order(int order)
{

}

}

bool register_menu_item(const std::string& item_name, std::function<void()> func)
{
    impl::init();

    impl::root_menu_item->items.emplace(0, impl::MenuItem::make_shared("Window", nullptr, 0));
    return true;
}



}
}