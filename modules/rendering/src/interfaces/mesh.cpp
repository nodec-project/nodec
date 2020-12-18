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
    target_rendering->unbind_mesh(this);
    nodec::logging::debug("sas", __FILE__, __LINE__);
}


}
}
}