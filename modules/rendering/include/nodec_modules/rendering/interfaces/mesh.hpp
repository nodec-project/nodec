#ifndef NODEC_MODULES__RENDERING__INTERFACES__MESH_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__MESH_HPP_

#include "bindable.hpp"

#include <nodec/nodec_object.hpp>
#include <nodec/vector3.hpp>

#include <vector>
#include <memory>
#include <unordered_map>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            class Rendering;

            class Mesh : public nodec::NodecObject, public IBindable
            {
            public:
                std::vector<nodec::Vector3f> vertices;
                std::vector<int> indicies;

            public:
                Mesh();
                ~Mesh();

            protected:
                void bind_impl(Rendering* rendering) override;
                void unbind_impl(Rendering* rendering) override;


            };
        }
    }
}

#endif
