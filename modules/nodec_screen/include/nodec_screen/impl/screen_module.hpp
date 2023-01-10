#ifndef NODEC_SCREEN__IMPL__SCREEN_MODULE_HPP_
#define NODEC_SCREEN__IMPL__SCREEN_MODULE_HPP_

#include "../screen.hpp"

#include <nodec/signals/signal.hpp>
#include <nodec/logging.hpp>

namespace nodec_screen {
namespace impl {

class ScreenModule : public Screen {
public:
    using ResolutionChangedSignal = nodec::signals::Signal<void(ScreenModule&, const nodec::Vector2i&)>;
    using SizeChangedSignal = nodec::signals::Signal<void(ScreenModule&, const nodec::Vector2i&)>;
    using TitleChangedSignal = nodec::signals::Signal<void(ScreenModule&, const std::string&)>;

public:
    ScreenModule() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[ScreenModule] >>> Created!";
    };

    nodec::Vector2i resolution() const noexcept override {
        return internal_resolution;
    };

    void set_resolution(const nodec::Vector2i& resolution) override {
        resolution_changed_(*this, resolution);
    }

    nodec::Vector2i size() const noexcept override {
        return internal_size;
    }

    void set_size(const nodec::Vector2i& size) override {
        size_changed_(*this, size);
    }

    std::string title() const noexcept override {
        return internal_title;
    }

    void set_title(const std::string& title) override {
        title_changed_(*this, title);
    }

public:
    decltype(auto) resolution_changed() { return resolution_changed_.signal_interface(); }

    decltype(auto) size_changed() { return size_changed_.signal_interface(); }

    decltype(auto) title_changed() { return title_changed_.signal_interface(); }

public:
    nodec::Vector2i internal_size;
    nodec::Vector2i internal_resolution;
    std::string internal_title;

private:
    ResolutionChangedSignal resolution_changed_;
    SizeChangedSignal size_changed_;
    TitleChangedSignal title_changed_;
};

}
}


#endif