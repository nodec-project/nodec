#ifndef NODEC_MODULES__SCREEN__INTERFACES__SCREEN_HPP_
#define NODEC_MODULES__SCREEN__INTERFACES__SCREEN_HPP_

#include <nodec/vector2.hpp>
#include <nodec/module_interface.hpp>

namespace nodec_modules {
namespace screen {
namespace interfaces {


class Screen : public nodec::ModuleInterface
{
public:
    using ModuleInterface::ModuleInterface;

public:
    virtual nodec::Vector2i resolution() const noexcept = 0;
    virtual void set_resolution(const nodec::Vector2i& resolution) = 0;

    virtual nodec::Vector2i size() const noexcept = 0;
    virtual void set_size(const nodec::Vector2i& size) = 0;

    virtual std::string title() const noexcept = 0;
    virtual void set_title(const std::string& title) = 0;

};


} // namespace interfaces
} // namespace screen
} // namespace nodec_modules


#endif