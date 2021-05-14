#ifndef SCREEN__IMPL__SCREEN_MODULE_HPP_
#define SCREEN__IMPL__SCREEN_MODULE_HPP_

#include <screen/screen.hpp>

#include <nodec/signals.hpp>

namespace screen {
namespace impl {

class ScreenModule : public Screen {
public:
    using ResolutionChangeSignal = nodec::signals::Signal<void(ScreenModule&, const nodec::Vector2i&)>;
    using SizeChangeSignal = nodec::signals::Signal<void(ScreenModule&, const nodec::Vector2i&)>;
    using TitleChangeSignal = nodec::signals::Signal<void(ScreenModule&, const std::string&)>;

public:
    ScreenModule() = default;

    nodec::Vector2i resolution() const noexcept override;
    void set_resolution(const nodec::Vector2i& resolution) override;

    nodec::Vector2i size() const noexcept override;
    void set_size(const nodec::Vector2i& size) override;

    std::string title() const noexcept override;
    void set_title(const std::string& title) override;

public:
    ResolutionChangeSignal::Interface& on_resolution_change() { return on_resolution_change_; }

    SizeChangeSignal::Interface& on_size_change() { return on_size_change_; }

    TitleChangeSignal::Interface& on_title_change() { return on_title_change_; }

public:
    nodec::Vector2i size_internal;
    nodec::Vector2i resolution_internal;
    std::string title_internal;

private:
    ResolutionChangeSignal on_resolution_change_;
    SizeChangeSignal on_size_change_;
    TitleChangeSignal on_title_change_;
};

}
}


#endif