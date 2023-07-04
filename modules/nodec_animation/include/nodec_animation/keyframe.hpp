#ifndef NODEC_ANIMATIONS__KEYFRAME_HPP_
#define NODEC_ANIMATIONS__KEYFRAME_HPP_

#include <cstdint>

namespace nodec_animation {

struct Keyframe {
    std::uint16_t ticks;
    float value;


    constexpr bool operator<(const Keyframe& other) const noexcept {
        return ticks < other.ticks;
    }

    constexpr bool operator>(const Keyframe& other) const noexcept {
        return ticks > other.ticks;
    }
};
}

#endif