#ifndef NODEC_RENDERING__COMPONENTS__TEXT_HPP_
#define NODEC_RENDERING__COMPONENTS__TEXT_HPP_

#include "../resources/font.hpp"
#include "../resources/material.hpp"

#include <string>
#include <memory>


namespace nodec_rendering {
namespace components {

struct Text {
    std::string text;

    std::shared_ptr<resources::Font> font;
    std::shared_ptr<resources::Material> material;

    int pixel_size{10};
};
}
} // namespace nodec_rendering

#endif