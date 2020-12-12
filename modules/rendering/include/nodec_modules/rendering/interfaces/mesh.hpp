#ifndef NODEC_MODULES__RENDERING__INTERFACES__MESH_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__MESH_HPP_

#include "bindable_resource.hpp"

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

class Mesh : public BindableResource
{
public:
    std::vector<nodec::Vector3f> vertices;
    std::vector<uint16_t> triangles;

public:
    Mesh(Rendering* target_rendering);
    ~Mesh();


};

}
}
}

#endif
