#include "imwindow/imwindow_impl.hpp"

#include "imgui.h"

#include <vector>

namespace nodec_modules
{
namespace imwindow
{
namespace interfaces
{

namespace imwindow_manager
{

void register_window(const ImWindow& window)
{

}

}

} // namespace interfaces

void show_window_main_menu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Window"))
        {

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

}
}