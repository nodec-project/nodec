#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

#include <nodec/logging.hpp>

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
    nodec::logging::debug("unbind mesh", __FILE__, __LINE__);
    target_rendering->unbind_mesh(this);
}


}
}
}