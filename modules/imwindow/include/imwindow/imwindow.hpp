#ifndef IMWINDOW__IMWINDOW_HPP_
#define IMWINDOW__IMWINDOW_HPP_

#include <nodec/macros.hpp>
#include <nodec/object.hpp>
#include <nodec/vector2.hpp>

#include <functional>
#include <unordered_map>

namespace imwindow
{

class ImWindow
{
private:
    NODEC_DISABLE_COPY(ImWindow)

public:
    std::string title;
    nodec::Vector2f init_size;

    ImWindow(const std::string& title, const nodec::Vector2f& init_size) :
        title(title),
        init_size(init_size)
    {
    };

    virtual void on_gui() {};
};

namespace impl
{

}

namespace detail
{
extern std::unordered_map<size_t, nodec::Object::Holder<ImWindow>> active_windows;
}

template<typename T>
nodec::Object::Reference<T> get_window()
{
    static_assert(std::is_base_of<ImWindow, T>::value, "T must be derived from ImWindow.");

    auto type_hash = typeid(T).hash_code();
    auto iter = detail::active_windows.find(type_hash);
    if (iter == detail::active_windows.end())
    {
        //auto window = std::dynamic_pointer_cast<ImWindow>(nodec::Object::make_holder<T>());
        //auto window = std::static_pointer_cast<ImWindow>(nodec::Object::make_holder<T>());
        auto window = nodec::Object::make_holder<T>();
        //using pair_t = std::pair<size_t, nodec::Object::Holder<ImWindow>>;
        //pair_t pair(type_hash, window);
        //detail::active_windows.insert(pair);
        //detail::active_windows.emplace(type_hash, window);
        detail::active_windows.emplace(type_hash, window);
        return window;
    }

    return std::dynamic_pointer_cast<T>(iter->second);
}





}


#endif