#ifndef NODEC_RENDERING__COMPONENTS__TEXT_RENDERER_HPP_
#define NODEC_RENDERING__COMPONENTS__TEXT_RENDERER_HPP_

#include "../resources/font.hpp"
#include "../resources/material.hpp"

#include <nodec/vector4.hpp>

#include <string>
#include <memory>


namespace nodec_rendering {
namespace components {

struct TextRenderer {
    std::string text;

    std::shared_ptr<resources::Font> font;
    std::shared_ptr<resources::Material> material;

    int pixel_size{10};
    int pixels_per_unit{10};

    nodec::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};
};
}
} // namespace nodec_rendering

#endif