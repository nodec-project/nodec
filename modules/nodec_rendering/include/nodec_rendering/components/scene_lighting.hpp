#ifndef NODEC_RENDERING__COMPONENTS__SCENE_LIGHTING_HPP_
#define NODEC_RENDERING__COMPONENTS__SCENE_LIGHTING_HPP_

#include "../resources/material.hpp"

#include <nodec/vector4.hpp>

#include <memory>

namespace nodec_rendering {
namespace components {

struct SceneLighting {
    nodec::Vector4f ambient_color{0.03f, 0.03f, 0.03f, 1.0f};

    std::shared_ptr<resources::Material> skybox;
};

} // namespace components
} // namespace nodec_rendering

#endif