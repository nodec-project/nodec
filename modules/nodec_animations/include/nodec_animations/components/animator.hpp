#ifndef NODEC_ANIMATIONS__COMPONENTS__ANIMATOR_HPP_
#define NODEC_ANIMATIONS__COMPONENTS__ANIMATOR_HPP_

#include <memory>

#include "../resources/animation_clip.hpp"

namespace nodec_animations {
namespace components {

struct Animator {
    std::shared_ptr<resources::AnimationClip> clip;

};

struct AnimatorBegin {};
}
} // namespace nodec_animations

#endif