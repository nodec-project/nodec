#ifndef NODEC_RENDERING__COMPONENTS__MESH_RENDERER_HPP_
#define NODEC_RENDERING__COMPONENTS__MESH_RENDERER_HPP_

#include "../resources/material.hpp"
#include "../resources/mesh.hpp"

#include <memory>
#include <vector>

namespace nodec_rendering {
namespace components {

struct MeshRenderer {
    std::vector<std::shared_ptr<resources::Mesh>> meshes;
    std::vector<std::shared_ptr<resources::Material>> materials;
};

} // namespace components
} // namespace nodec_rendering

#endif