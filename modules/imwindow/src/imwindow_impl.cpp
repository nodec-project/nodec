#include <imwindow/impl/imwindow_impl.hpp>

#include <imgui.h>

#include <nodec/nodec_exception.hpp>

#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>


namespace imwindow
{
namespace impl
{


std::unordered_map<size_t, std::function<std::shared_ptr<ImWindow>()>> registered_windows;



void register_window(size_t type_hash, std::function<std::shared_ptr<ImWindow>()> func)
{
    if (registered_windows.find(type_hash) == registered_windows.end())
    {
        throw nodec::NodecException("Already registered.", __FILE__, __LINE__);
    }

    auto result = registered_windows.emplace(type_hash, func);
    if (!result.second)
    {
        throw nodec::NodecException("Failed to register.", __FILE__, __LINE__);
    }
}


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
