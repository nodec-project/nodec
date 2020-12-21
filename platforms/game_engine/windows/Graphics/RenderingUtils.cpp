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
    try
    {
        std::vector<uint8_t> cbuffer;
        CreateMaterialCBuffer(material, cbuffer);
        auto constantBuffer = std::make_shared<ConstantBuffer>(graphics,
                                                               cbuffer.size(),
                                                               cbuffer.data());
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

template<typename T>
static void AppendPropertiesByteSequence(const std::map<std::string, T>& properties, std::vector<uint8_t>& cbuffer)
{
    for (auto& pair : properties)
    {
        // <https://stackoverrun.com/ja/q/3787586>
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&pair.second);
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            cbuffer.push_back(p[i]);
        }

        constexpr size_t alignment_size = 16;
        size_t aligned = (cbuffer.size() + (alignment_size - 1)) & ~(alignment_size - 1);

        // add padding so that size will be a multiple of 16.
        while (cbuffer.size() < aligned)
        {
            cbuffer.push_back(0x00);
        }
    }
}

void CreateMaterialCBuffer(const nodec_modules::rendering::interfaces::Material* material,
                                  std::vector<uint8_t>& cbuffer)
{
    AppendPropertiesByteSequence(material->float_properties(), cbuffer);
    AppendPropertiesByteSequence(material->vector4_properties(), cbuffer);
    
    

    //nodec::logging::DebugStream(__FILE__, __LINE__) << cbuffer.size() << std::flush;
}


} // namespace RenderingUtils