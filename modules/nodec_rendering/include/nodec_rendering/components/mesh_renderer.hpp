#ifndef NODEC_RENDERING__COMPONENTS__MESH_RENDERER_HPP_
#define NODEC_RENDERING__COMPONENTS__MESH_RENDERER_HPP_

#include "../resources/material.hpp"
#include "../resources/mesh.hpp"

#include <memory>
#include <vector>

namespace nodec_rendering {
namespace components {

struct MeshRenderer {
    using SharedMesh = std::shared_ptr<resources::Mesh>;
    using SharedMaterial = std::shared_ptr<resources::Material>;

    std::vector<SharedMesh> meshes;
    std::vector<SharedMaterial> materials;
};

} // namespace components
} // namespace nodec_rendering

#endif