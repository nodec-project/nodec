#ifndef NODEC_RENDERING__COMPONENTS__IMAGE_RENDERER_HPP_
#define NODEC_RENDERING__COMPONENTS__IMAGE_RENDERER_HPP_

#include "../nodec_rendering.hpp"
#include "../resources/material.hpp"

#include <memory>

namespace nodec_rendering {
namespace components {

struct ImageRenderer {
    using MaterialPtr = std::shared_ptr<resources::Material>;
    using TexturePtr = std::shared_ptr<resources::Texture>;

    TexturePtr image;
    MaterialPtr material;

    int pixelsPerUnit{100};
};

} // namespace components
} // namespace nodec_rendering
#endif