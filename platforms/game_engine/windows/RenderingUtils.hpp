#pragma once

#include "Graphics.hpp"
#include "GraphicsResources.hpp"

#include <nodec_modules/rendering/interfaces/mesh.hpp>


void PublishMesh(nodec_modules::rendering::interfaces::Mesh* mesh, Graphics* graphics, GraphicsResources* graphicsResources);