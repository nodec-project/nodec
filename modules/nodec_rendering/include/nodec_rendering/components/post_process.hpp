#ifndef NODEC_RENDERING__COMPONENTS__POST_PROCESS_HPP_
#define NODEC_RENDERING__COMPONENTS__POST_PROCESS_HPP_

#include "../resources/material.hpp"

#include <memory>
#include <vector>

namespace nodec_rendering {

namespace components {

struct PostProcess {
    using MaterialSharedPtr = std::shared_ptr<resources::Material>;

    std::vector<MaterialSharedPtr> materials;
};
} // namespace components

} // namespace nodec_rendering

#endif