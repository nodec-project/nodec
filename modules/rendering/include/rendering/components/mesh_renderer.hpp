#ifndef RENDERING__COMPONENTS__MESH_RENDERER_HPP_
#define RENDERING__COMPONENTS__MESH_RENDERER_HPP_

#include "../resources/mesh.hpp"
#include "../resources/material.hpp"

#include <vector>
#include <memory>


namespace rendering {
namespace components {

struct MeshRenderer {
    using SharedMesh = std::shared_ptr<resources::Mesh>;
    using SharedMaterial = std::shared_ptr<resources::Material>;

    std::vector<SharedMesh> meshes;
    std::vector<SharedMaterial> materials;
};

}
}


#endif