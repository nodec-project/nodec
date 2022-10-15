#ifndef NODEC_ANIMATIONS__ANIMATION_CURVE_HPP_
#define NODEC_ANIMATIONS__ANIMATION_CURVE_HPP_

#include "keyframe.hpp"
#include "wrap_mode.hpp"

#include <nodec/algorithm.hpp>

#include <vector>

namespace nodec_animations {

class AnimationCurve {
public:
    void set_wrap_mode(const WrapMode &mode) {
        wrap_mode_ = mode;
    }

    int add_keyframe(const Keyframe &keyframe) {
        auto iter = std::lower_bound(keyframes_.begin(), keyframes_.end(), keyframe);
        iter = keyframes_.insert(iter, keyframe);
        return static_cast<int>(std::distance(keyframes_.begin(), iter));
    }

    const Keyframe &operator[](std::size_t index) const {
        return keyframes_[index];
    }

    std::pair<int, float> evaluate(std::uint16_t ticks, int hint = -1) {
        if (keyframes_.size() == 0) return std::make_pair(-1, 0.0f);

        Keyframe current;
        current.ticks = [&]() {
            switch (wrap_mode_) {
            case WrapMode::Once:
            default:
                return nodec::clamp(ticks,
                                    static_cast<std::uint16_t>(0),
                                    static_cast<std::uint16_t>(keyframes_.size() - 1));

            case WrapMode::Loop:
                return static_cast<std::uint16_t>(ticks % keyframes_.size());
            }
        }();

        auto iter = [&]() {
            if (hint < 0 || keyframes_.size() - 1 <= hint) {
                return std::lower_bound(keyframes_.begin(), keyframes_.end(), current);
            }

            if (ticks < keyframes_[hint].ticks) {
                if (keyframes_[hint - 1].ticks <= ticks) {
                    return keyframes_.begin() + hint - 1;
                }

                return std::lower_bound(keyframes_.begin(), keyframes_.begin() + hint - 1, current);
            }

            if (ticks < keyframes_[hint + 1].ticks) {
                return keyframes_.begin() + hint + 1;
            }

            if (keyframes_.size() <= hint + 2) {
                return keyframes_.end();
            }

            if (ticks < keyframes_[hint + 2].ticks) {
                return keyframes_.begin() + hint + 2;
            }

            return std::lower_bound(keyframes_.begin() + hint + 2, keyframes_.end(), current);
        }();

    }

private:
    std::vector<Keyframe> keyframes_;
    WrapMode wrap_mode_{WrapMode::Once};
};
} // namespace nodec_animations

#endif