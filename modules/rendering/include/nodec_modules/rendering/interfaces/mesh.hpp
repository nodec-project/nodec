#ifndef NODEC_MODULES__RENDERING__INTERFACES__MESH_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__MESH_HPP_


#include <nodec/nodec_object.hpp>
#include <nodec/vector3.hpp>

#include <vector>
#include <memory>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            class Mesh : public nodec::NodecObject
            {
            public:
                std::vector<nodec::Vector3f> vertices;

            public:
                Mesh();
                ~Mesh();
            };
        }
    }
}

#endif
