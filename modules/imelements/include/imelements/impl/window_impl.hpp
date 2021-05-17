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
        for (auto& pair : active_windows) {
            auto& window = pair.second;

            ImGui::SetNextWindowSize(ImVec2(window->init_size.x, window->init_size.y), ImGuiCond_FirstUseEver);
            if (ImGui::Begin(window->title.c_str())) {
                window->on_gui();
            }
            ImGui::End();
        }
    }
};

}
}


#endif