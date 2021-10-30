#ifndef RENDERING__COMPONENTS__MESH_RENDERER_HPP_
#define RENDERING__COMPONENTS__MESH_RENDERER_HPP_

#include <rendering/resources/mesh.hpp>

#include <memory>


namespace rendering {
namespace components {

struct MeshRenderer {

    std::shared_ptr<resources::Mesh> mesh;

};

}
}


#endif