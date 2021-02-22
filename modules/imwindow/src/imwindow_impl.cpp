#include <imwindow/impl/imwindow_impl.hpp>

#include <imgui.h>

#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>


namespace imwindow
{
namespace impl{



void register_window(size_t type_hash, std::function<std::shared_ptr<ImWindow>()> func)
{

}

std::unordered_map<size_t, std::function<std::shared_ptr<ImWindow>()>> registered_windows;

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

} // namespace impl
}
