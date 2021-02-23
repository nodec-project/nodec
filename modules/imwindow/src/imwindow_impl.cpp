#include <imwindow/impl/imwindow_impl.hpp>

#include <imgui.h>

#include <nodec/nodec_exception.hpp>
#include <nodec/object.hpp>

#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>


namespace imwindow
{

namespace detail
{
std::unordered_map<size_t, nodec::Object::Holder<ImWindow>> active_windows;
}

namespace impl
{

void update_windows()
{
    for (auto& pair : detail::active_windows)
    {
        auto window = pair.second;

        ImGui::SetNextWindowSize(ImVec2(window->init_size.x, window->init_size.y), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(window->title.c_str()))
        {
            window->on_gui();
        }
        ImGui::End();

    }
}

} // namespace impl

}
