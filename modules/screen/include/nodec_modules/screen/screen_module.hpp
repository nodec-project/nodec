#ifndef NODEC_MODULES__SCREEN__SCREEN_MODULE_HPP_
#define NODEC_MODULES__SCREEN__SCREEN_MODULE_HPP_

#include "interfaces/screen.hpp"

#include <nodec/event.hpp>

namespace nodec_modules {
namespace screen {

class ScreenModule : public interfaces::Screen {
public:
    ScreenModule();

public:
    nodec::Vector2i resolution() const noexcept override;
    void set_resolution(const nodec::Vector2i& resolution) override;

    nodec::Vector2i size() const noexcept override;
    void set_size(const nodec::Vector2i& size) override;

    std::string title() const noexcept override;
    void set_title(const std::string& title) override;

public:
    nodec::Vector2i size_;
    nodec::Vector2i resolution_;
    std::string title_;

    nodec::event::Event<ScreenModule&> on_submit_resolution_change;
    nodec::event::Event<ScreenModule&> on_submit_size_change;
    nodec::event::Event<ScreenModule&> on_submit_title_change;

};

}
}

#endif