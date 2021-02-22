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

static void show_menu_items(const MenuItem& item)
{
    for (auto& pair : item.items)
    {
        auto& item = *(pair.second);
        if (item.items.empty())
        {
            if (ImGui::MenuItem(item.name.c_str()))
            {

            }

            continue;
        }

        if (ImGui::BeginMenu(item.name.c_str()))
        {
            show_menu_items(item);
            ImGui::EndMenu();
        }
    }
}

void show_main_menu()
{
    init();

    if (ImGui::BeginMainMenuBar())
    {
        show_menu_items(*root_menu_item);


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

bool register_menu_item(const std::string& item_name, std::function<void()> func, int order = 0)
{
    impl::init();

    auto current_item = impl::root_menu_item.get();
    size_t offset = 0;
    while (true)
    {
        auto pos = item_name.find_first_of('/', offset);
        if (pos == std::string::npos)
        {
            break;
        }
        auto name = item_name.substr(offset, pos - offset);

        auto next_item = current_item;
        for (auto& pair : current_item->items)
        {
            if (pair.second->name == name && !pair.second->items.empty())
            {
                next_item = pair.second.get();
            }
        }

        if (next_item == current_item)
        {
            auto iter = current_item->items.emplace(0, impl::MenuItem::make_shared(name.c_str(), nullptr, 0));
            next_item = iter->second.get();
        }

        current_item = next_item;
        offset = pos + 1;
    }

    auto name = item_name.substr(offset);

    current_item->items.emplace(order, impl::MenuItem::make_shared(name, func, order));
    return true;
}



}
}