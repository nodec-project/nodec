#include <screen/impl/screen_module.hpp>


namespace screen {
namespace impl {


nodec::Vector2i ScreenModule::resolution() const noexcept {
    return resolution_internal;
}

void ScreenModule::set_resolution(const nodec::Vector2i& resolution) {
    on_resolution_change_(*this, resolution);
}

nodec::Vector2i ScreenModule::size() const noexcept {
    return size_internal;
}

void ScreenModule::set_size(const nodec::Vector2i& size) {
    on_size_change_(*this, size);
}

std::string ScreenModule::title() const noexcept {
    return title_internal;
}

void ScreenModule::set_title(const std::string& title) {
    on_title_change_(*this, title);
}

}
}