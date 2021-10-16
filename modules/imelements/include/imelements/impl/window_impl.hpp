#ifndef IMELEMENTS__IMPL__WINDOW_IMPL_HPP_
#define IMELEMENTS__IMPL__WINDOW_IMPL_HPP_

#include <imelements/window.hpp>

#include <imgui.h>

namespace imelements {

namespace impl {



class WindowManagerImpl : public WindowManager {
public:
    WindowManagerImpl() = default;

public:
    void update_windows() {
        for (auto iter = active_windows.begin(); iter != active_windows.end();) {
            auto& window = iter->second;

            if (window->is_closed()) {
                iter = active_windows.erase(iter);
                continue;
            }

            bool is_shown;
            ImGui::SetNextWindowSize(ImVec2(window->init_size.x, window->init_size.y), ImGuiCond_Once);
            if (ImGui::Begin(window->name(), &is_shown)) {

                // ToDO: Error handling
                window->on_gui();


            }
            ImGui::End();

            if (!is_shown) {
                window->close();
            }
            ++iter;
        }
    }
};

}
}


#endif