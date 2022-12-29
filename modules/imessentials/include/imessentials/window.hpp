#ifndef IMESSENTIALS__WINDOW_HPP_
#define IMESSENTIALS__WINDOW_HPP_

#include <nodec/macros.hpp>
#include <nodec/type_info.hpp>
#include <nodec/vector2.hpp>

#include <cassert>
#include <functional>
#include <unordered_map>

#include <imgui.h>

namespace imessentials {

class BaseWindow {
public:
    nodec::Vector2f init_size;

public:
    BaseWindow(const char *name, const nodec::Vector2f &init_size)
        : name_(name), init_size(init_size), is_closed_(false){};
    virtual ~BaseWindow() {}

    virtual void on_gui() = 0;

    void close() {
        is_closed_ = true;
    }

    bool is_closed() {
        return is_closed_;
    }
    const char *name() {
        return name_;
    }

    void focus() const {
        ImGui::SetWindowFocus(name_);
    }

private:
    const char *name_;
    bool is_closed_;

private:
    NODEC_DISABLE_COPY(BaseWindow)
};

class WindowManager {
public:
    WindowManager() = default;
    virtual ~WindowManager() {}

public:
    template<typename T, typename MakeFunc>
    void register_window(MakeFunc &&make_func) {
        static_assert(std::is_base_of<BaseWindow, T>::value, "T must be derived from BaseWindow.");

        auto &info = nodec::type_id<T>();

        assert(window_makers_.find(info) == window_makers_.end());

        window_makers_[info] = make_func;
    }

    template<typename T>
    T &get_window() {
        static_assert(std::is_base_of<BaseWindow, T>::value, "T must be derived from BaseWindow.");

        auto &info = nodec::type_id<T>();

        {
            auto iter = active_windows_.find(info);
            if (iter != active_windows_.end()) {
                return static_cast<T &>(*iter->second);
            }
        }

        // Not active, make the window.
        auto window = window_makers_[info]();
        assert(window);

        auto iter = active_windows_.emplace(info, std::move(window)).first;
        return static_cast<T &>(*iter->second);
    }

protected:
    std::unordered_map<nodec::type_info, std::unique_ptr<BaseWindow>> active_windows_;
    std::unordered_map<nodec::type_info, std::function<std::unique_ptr<BaseWindow>()>> window_makers_;

private:
    NODEC_DISABLE_COPY(WindowManager)
};

} // namespace imessentials

#endif