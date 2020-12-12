#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

Mesh::Mesh(Rendering* target_rendering) :
    BindableResource(target_rendering, "Mesh")
{

}

Mesh::~Mesh()
{
    target_rendering->unbind_mesh(this);
}


}
}
}