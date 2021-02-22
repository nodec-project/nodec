#ifndef GAME_EDITOR__MENU_HPP_
#define GAME_EDITOR__MENU_HPP_

#include <functional>
#include <string>


namespace game_editor
{

namespace menu
{

bool register_menu_item(const std::string& item_name, std::function<void()> func, int order = 0);


}
}


#endif