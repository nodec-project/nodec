#pragma once

#include <nodec_scene/scene_registry.hpp>
#include <nodec_scene/scene.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>
#include <nodec_scene_serialization/serializable_scene_graph.hpp>

#include <nodec/resource_management/resource_registry.hpp>


namespace ResourceImporter {


namespace internal {

void ProcessSceneEntityNode(
    std::shared_ptr<nodec_scene_serialization::SerializableEntityNode> node,
    const nodec_scene_serialization::SceneSerialization& sceneSerialization,
    nodec_scene::SceneEntity parent,
    nodec_scene::Scene& scene)
{
    using namespace nodec_scene::components;
    using namespace nodec::entities;
    auto entity = sceneSerialization.emplace_entity(node, scene.registry());

    if (parent != null_entity) {
        scene.append_child(parent, entity);
    }
    else {
        scene.registry().emplace_component<Hierarchy>(entity);
    }

    for (auto child : node->children) {
        ProcessSceneEntityNode(child, sceneSerialization, entity, scene);
    }
}

}

bool ImportSceneGraph(
    const std::string& path,
    nodec_scene::SceneEntity destEntity,
    nodec::resource_management::ResourceRegistry& resourceRegistry,
    nodec_scene::Scene& scene,
    const nodec_scene_serialization::SceneSerialization& sceneSerialization)
{
    using namespace nodec;
    using namespace nodec_scene_serialization;

    auto graph = resourceRegistry.get_resource<SerializableSceneGraph>(path).get();

    if (!graph) {
        return false; 
    }

    for (auto node : graph->roots) {
        internal::ProcessSceneEntityNode(node, sceneSerialization, destEntity, scene);
    }

    return true;
}

}
