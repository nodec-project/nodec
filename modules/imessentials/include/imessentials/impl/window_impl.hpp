#ifndef IMESSENTIALS__IMPL__WINDOW_IMPL_HPP_
#define IMESSENTIALS__IMPL__WINDOW_IMPL_HPP_

#include <imessentials/window.hpp>

#include <nodec/logging.hpp>

namespace imessentials {
namespace impl {

class WindowManagerImpl final : public WindowManager {
public:
    WindowManagerImpl() = default;

public:
    void update_windows() {
        for (auto iter = active_windows_.begin(); iter != active_windows_.end();) {
            auto &window = iter->second;

            if (window->is_closed()) {
                iter = active_windows_.erase(iter);
                continue;
            }

            bool is_shown = true;
            ImGui::SetNextWindowSize(ImVec2(window->init_size.x, window->init_size.y), ImGuiCond_FirstUseEver);
            if (ImGui::Begin(window->name(), &is_shown)) {
                try {
                    window->on_gui();
                } catch (...) {
                    handle_exception(window->name());
                }
            }
            ImGui::End();

            if (!is_shown) {
                window->close();
            }
            ++iter;
        }
    }

private:
    static void handle_exception(const char *name) {
        try {
            throw;
        } catch (std::exception &e) {
            nodec::logging::ErrorStream(__FILE__, __LINE__)
                << "An exception was thrown in '" << name << "'\n"
                << "details: \n"
                << e.what();
        } catch (...) {
            nodec::logging::ErrorStream(__FILE__, __LINE__)
                << "An unknown exception was thrown in '" << name;
        }
    }
};

} // namespace impl
} // namespace imessentials

#endif