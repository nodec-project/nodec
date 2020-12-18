#include "RenderingUtils.hpp"

#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec_modules/rendering/interfaces/shader.hpp>
#include <nodec_modules/rendering/interfaces/material.hpp>

using namespace nodec_modules;
using namespace nodec;


/**
* @note
*   <https://stackoverflow.com/questions/3561659/how-can-i-abstract-out-a-repeating-try-catch-pattern-in-c>
*/
void HandleException(const char* blockName)
{
    try
    {
        throw;
    }
    catch (const NodecException& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << "A NodecException has been occured while '" << (blockName) << "'.\n"
            << "error_message: " << e.what() << std::flush;
    }
    catch (const std::exception& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << "A StandardException has been occured while '" << (blockName) << "'.\n"
            << "error_message: " << e.what() << std::flush;
    }
    catch (...)
    {
        logging::ErrorStream(__FILE__, __LINE__) << "A UnknownException has been occured while '" << (blockName) << "'.\n"
            << "error_message: Unavailable." << std::flush;
    }
}

namespace RenderingUtils
{

void BindMesh(const nodec_modules::rendering::interfaces::Mesh* mesh,
              Graphics* graphics, GraphicsResources* graphicsResources)
{

    if (mesh->vertices.size() > 0
        && graphicsResources->vertexBufferMap.find(mesh->id()) == graphicsResources->vertexBufferMap.end())
    {
        try
        {
            auto vertexBuffer = std::make_shared<VertexBuffer>(
                graphics,
                mesh->vertices.size() * sizeof(rendering::interfaces::Mesh::Vertex),
                sizeof(rendering::interfaces::Mesh::Vertex),
                mesh->vertices.data()
                );
            graphicsResources->vertexBufferMap.emplace(mesh->id(), vertexBuffer);
        }
        catch (...)
        {
            HandleException("Creating VertexBuffer");
        }
    }

    if (mesh->triangles.size() > 0
        && graphicsResources->indexBufferMap.find(mesh->id()) == graphicsResources->indexBufferMap.end())
    {
        try
        {
            auto indexBuffer = std::make_shared<IndexBuffer>(
                graphics,
                mesh->triangles.size(),
                mesh->triangles.data()
                );
            graphicsResources->indexBufferMap.emplace(mesh->id(), indexBuffer);
        }
        catch (...)
        {
            HandleException("Creating IndexBuffer");
        }
    }
}

void UnbindMesh(const nodec_modules::rendering::interfaces::Mesh* mesh,
                GraphicsResources* graphicsResources)
{
    graphicsResources->indexBufferMap.erase(mesh->id());
    graphicsResources->vertexBufferMap.erase(mesh->id());
}

void BindShader(const nodec_modules::rendering::interfaces::Shader* shader,
                Graphics* graphics,
                GraphicsResources* graphicsResources)
{
    nodec::logging::debug("stst", __FILE__, __LINE__);
    std::string vertexShaderPath = "resources/shaders/";
    vertexShaderPath += shader->shader_name();
    vertexShaderPath += "/vertex.cso";

    std::string pixelShaderPath = "resources/shaders/";
    pixelShaderPath += shader->shader_name();
    pixelShaderPath += "/pixel.cso";

    const D3D11_INPUT_ELEMENT_DESC ied[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                           , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    try
    {
        auto vertexShader = std::make_shared<VertexShader>(graphics, vertexShaderPath, ied, std::size(ied));
        graphicsResources->vertexShaderMap.emplace(shader->id(), vertexShader);
    }
    catch(...)
    {
        HandleException("Creating VertexShader");
    }

    try
    {
        auto pixelShader = std::make_shared<PixelShader>(graphics, pixelShaderPath);
        graphicsResources->pixelShaderMap.emplace(shader->id(), pixelShader);
    }
    catch (...)
    {
        HandleException("Creating PixelShader");
    }

}

void UnbindShader(const nodec_modules::rendering::interfaces::Shader* shader,
                  GraphicsResources* graphicsResources)
{
    graphicsResources->vertexShaderMap.erase(shader->id());
    graphicsResources->pixelShaderMap.erase(shader->id());
}

void BindMaterial(const nodec_modules::rendering::interfaces::Material* material,
                  Graphics* graphics,
                  GraphicsResources* graphicsResources)
{
    nodec::logging::DebugStream(__FILE__, __LINE__) << material->primitive_properties_byte_size() << std::flush;
    nodec::logging::DebugStream(__FILE__, __LINE__) << sizeof(float) << std::flush;

    try
    {
        auto constantBuffer = std::make_shared<ConstantBuffer>(graphics, 
                                                               material->primitive_properties_byte_size(), 
                                                               material->primitive_properties_entry_ptr());
        graphicsResources->constantBufferMap.emplace(material->id(), constantBuffer);

    }
    catch (...)
    {
        HandleException("Creating ConstantBuffer");
    }
}

void UnbindMaterial(const nodec_modules::rendering::interfaces::Material* material,
                    GraphicsResources* graphicsResources)
{
    graphicsResources->constantBufferMap.erase(material->id());
}

} // namespace RenderingUtils