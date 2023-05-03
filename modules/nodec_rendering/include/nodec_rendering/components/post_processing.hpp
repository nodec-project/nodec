#ifndef NODEC_RENDERING__COMPONENTS__POST_PROCESS_HPP_
#define NODEC_RENDERING__COMPONENTS__POST_PROCESS_HPP_

#include "../resources/material.hpp"

#include <memory>
#include <vector>

namespace nodec_rendering {

namespace components {

struct PostProcessing {
    struct Effect {
        bool enabled;
        std::shared_ptr<resources::Material> material;
    };

    std::vector<Effect> effects;
};
} // namespace components

} // namespace nodec_rendering

#endif