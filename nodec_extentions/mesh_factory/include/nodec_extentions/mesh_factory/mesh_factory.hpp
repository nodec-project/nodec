#ifndef NODEC_EXTENTIONS__MESH_FACTORY__MESH_FACTORY_HPP_
#define NODEC_EXTENTIONS__MESH_FACTORY__MESH_FACTORY_HPP_

#include <nodec_modules/rendering/interfaces/mesh.hpp>

namespace nodec_extentions
{
namespace mesh_factory
{

void make_cube(int divisions,nodec_modules::rendering::interfaces::Mesh& mesh);



} // namespace mesh_factory
} // namespace nodec_extentions

#endif