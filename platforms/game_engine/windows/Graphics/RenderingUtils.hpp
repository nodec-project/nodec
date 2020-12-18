#pragma once

#include "Graphics.hpp"
#include "GraphicsResources.hpp"
#include "RenderingHandlers.hpp"

#include <nodec_modules/rendering/rendering_module.hpp>

namespace RenderingUtils
{

void BindMesh(const nodec_modules::rendering::interfaces::Mesh* mesh,
              Graphics* graphics,
              GraphicsResources* graphicsResources);

void UnbindMesh(const nodec_modules::rendering::interfaces::Mesh* mesh,
                GraphicsResources* graphicsResources);

void BindShader(const nodec_modules::rendering::interfaces::Shader* shader,
                Graphics* graphics,
                GraphicsResources* graphicsResources);

void UnbindShader(const nodec_modules::rendering::interfaces::Shader* shader,
                  GraphicsResources* graphicsResources);

void BindMaterial(const nodec_modules::rendering::interfaces::Material* material,
                  Graphics* graphics,
                  GraphicsResources* graphicsResources);

void UnbindMaterial(const nodec_modules::rendering::interfaces::Material* material,
                    GraphicsResources* graphicsResources);

}
