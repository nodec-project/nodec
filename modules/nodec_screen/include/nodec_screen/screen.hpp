#ifndef NODEC_SCREEN__SCREEN_HPP_
#define NODEC_SCREEN__SCREEN_HPP_

#include <nodec/vector2.hpp>
#include <nodec/macros.hpp>

namespace nodec_screen {


class Screen {
public:
    Screen() = default;

    virtual nodec::Vector2i resolution() const noexcept = 0;
    virtual void set_resolution(const nodec::Vector2i& resolution) = 0;

    virtual nodec::Vector2i size() const noexcept = 0;
    virtual void set_size(const nodec::Vector2i& size) = 0;

    virtual std::string title() const noexcept = 0;
    virtual void set_title(const std::string& title) = 0;

private:
    NODEC_DISABLE_COPY(Screen)
};


} // namespace screen


#endif