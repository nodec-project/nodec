#ifndef NODEC_ANIMATIONS__RESOURCES__ANIMATION_CLIP_HPP_
#define NODEC_ANIMATIONS__RESOURCES__ANIMATION_CLIP_HPP_

#include "../animation_curve.hpp"

#include <string>
#include <cstdint>

namespace nodec_animations {
namespace resources {

class AnimationClip {

public:
    AnimationClip() {

    }

    template<class Component>
    void set_curve(const std::string& relative_path, const std::string &property_name, const AnimationCurve &curve) {

    }

private:
    std::uint16_t ticks_per_second;
};
}
}

#endif