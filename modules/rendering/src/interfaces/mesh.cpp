#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            Mesh::Mesh():
                NodecObject("Mesh")
            {

            }

            Mesh::~Mesh()
            {
                unbind();
            }

            void Mesh::bind_impl(Rendering* rendering)
            {
                rendering->bind_mesh(this);
            }

            void Mesh::unbind_impl(Rendering* rendering)
            {
                rendering->unbind_mesh(this);
            }

        }
    }
}