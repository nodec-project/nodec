#ifndef NODEC_RENDERING__COMPONENTS__LIGHT_HPP_
#define NODEC_RENDERING__COMPONENTS__LIGHT_HPP_

#include <nodec/vector4.hpp>

namespace nodec_rendering {
namespace components {

enum class LightType {
    Directional,
    Point,
    Spot
};

class Light {
public:
    LightType type{LightType::Directional};
    nodec::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};
    float intensity{1.0f};
};

} // namespace components
} // namespace nodec_rendering

#endif