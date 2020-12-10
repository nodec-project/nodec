#include "RenderingHandlers.hpp"
#include "RenderingUtils.hpp"

RenderingHandlers::RenderingHandlers(Graphics* pGraphics, GraphicsResources* pGraphicsResources):
    pGraphics(pGraphics),
    pGraphicsResources(pGraphicsResources)
{
    
}


void RenderingHandlers::HandleMeshBinding(nodec_modules::rendering::interfaces::Mesh* mesh)
{

    RenderingUtils::BindMesh(mesh, pGraphics, pGraphicsResources);

}