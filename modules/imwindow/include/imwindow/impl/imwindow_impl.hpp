#ifndef IMWINDOW__IMPL_IMWINDOW_IMPL_HPP_
#define IMWINDOW__IMPL_IMWINDOW_IMPL_HPP_

#include <imwindow/imwindow.hpp>

namespace imwindow
{

namespace impl
{

void register_window(size_t type_hash);

void show_window_main_menu();

void update_windows();

}


}


#endif