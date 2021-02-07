#ifndef NODEC_MODULES__IMWINDOW__IMWINDOW_IMPL_HPP_
#define NODEC_MODULES__IMWINDOW__IMWINDOW_IMPL_HPP_

#include "interfaces/imwindow.hpp"

namespace nodec_modules
{
namespace imwindow
{
namespace interfaces
{

void register_window(const ImWindow& window);



} // namespace interfaces

void show_window_main_menu();

void update_windows();


}
}

#endif