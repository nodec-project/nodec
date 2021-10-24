#ifndef NODEC_MODULES__RENDERING__INTERFACES__MESH_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__MESH_HPP_

#include "bindable_resource.hpp"

#include <nodec/nodec_object.hpp>
#include <nodec/vector3.hpp>
#include <nodec/vector2.hpp>

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
    struct Vertex
    {
        nodec::Vector3f position;
        nodec::Vector3f normal;
        nodec::Vector2f uv;
        nodec::Vector3f tangent;

        Vertex(const nodec::Vector3f& position) :
            position(position)
        {
        };

        Vertex(const nodec::Vector3f& position, const nodec::Vector3f& normal) :
            position(position),
            normal(normal)
        {
        };

        Vertex(const nodec::Vector3f& position, const nodec::Vector3f& normal, const nodec::Vector2f& uv) :
            position(position),
            normal(normal),
            uv(uv)
        {
        };

    };

    std::vector<Vertex> vertices;

    std::vector<uint16_t> triangles;


public:
    Mesh(Rendering* target_rendering);
    ~Mesh();


};

}
}
}

#endif
