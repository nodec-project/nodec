#ifndef IMESSENTIALS__MENU_HPP_
#define IMESSENTIALS__MENU_HPP_

#include <functional>
#include <string>

namespace imessentials {

bool register_menu_item(const std::string &item_name, std::function<void()> func, int order = 0);

}

#endif