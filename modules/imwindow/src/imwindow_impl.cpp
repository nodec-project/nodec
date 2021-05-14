#include <imwindow/impl/imwindow_impl.hpp>

#include <imgui.h>


#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>


namespace imwindow {

namespace details {
std::unordered_map<size_t, std::shared_ptr<ImWindow>> active_windows;
}

namespace impl {

void update_windows() {
    for (auto& pair : details::active_windows) {
        auto window = pair.second;

        ImGui::SetNextWindowSize(ImVec2(window->init_size.x, window->init_size.y), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(window->title.c_str())) {
            window->on_gui();
        }
        ImGui::End();

    }
}

} // namespace impl

}
