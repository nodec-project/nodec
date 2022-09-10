#ifndef IMESSENTIALS__WINDOW_HPP_
#define IMESSENTIALS__WINDOW_HPP_

#include <nodec/macros.hpp>
#include <nodec/vector2.hpp>

#include <functional>
#include <typeindex>
#include <unordered_map>

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

private:
    const char *name_;
    bool is_closed_;

private:
    NODEC_DISABLE_COPY(BaseWindow)
};

class WindowManager {
public:
    WindowManager() = default;

public:
    template<typename T>
    T &get_window() {
        static_assert(std::is_base_of<BaseWindow, T>::value, "T must be derived from BaseWindow.");

        auto iter = active_windows.find(typeid(T));
        if (iter == active_windows.end()) {
            // auto window = std::make_unique<T>();
            auto pair = active_windows.emplace(typeid(T), std::make_unique<T>());
            return static_cast<T &>(*pair.first->second);
            // return *window;
        }

        // auto &test = *iter->second;
        return static_cast<T &>(*iter->second);
    }

protected:
    std::unordered_map<std::type_index, std::unique_ptr<BaseWindow>> active_windows;

private:
    NODEC_DISABLE_COPY(WindowManager);
};

} // namespace imessentials

#endif