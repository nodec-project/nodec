#ifndef IMELEMENTS__WINDOW_HPP_
#define IMELEMENTS__WINDOW_HPP_

#include <nodec/macros.hpp>
#include <nodec/vector2.hpp>

#include <functional>
#include <unordered_map>
#include <typeindex>


namespace imelements {

class BaseWindow {
public:
    std::string title;
    nodec::Vector2f init_size;

public:
    BaseWindow(const std::string& title, const nodec::Vector2f& init_size) :
        title(title),
        init_size(init_size) {
    };

    virtual void on_gui() {};

private:
    NODEC_DISABLE_COPY(BaseWindow)
};


class WindowManager {
public:
    WindowManager() = default;
public:
    template<typename T>
    T& get_window() {
        static_assert(std::is_base_of<BaseWindow, T>::value, "T must be derived from BaseWindow.");

        auto iter = active_windows.find(typeid(T));
        if (iter == active_windows.end()) {
            //auto window = std::make_unique<T>();
            auto pair = active_windows.emplace(typeid(T), std::make_unique<T>());
            return static_cast<T&>(*pair.first->second);
            //return *window;
        }

        //auto &test = *iter->second;
        return static_cast<T&>(*iter->second);
    }


protected:
    std::unordered_map<std::type_index, std::unique_ptr<BaseWindow>> active_windows;

private:
    NODEC_DISABLE_COPY(WindowManager);
};


} // namespace imelements


#endif