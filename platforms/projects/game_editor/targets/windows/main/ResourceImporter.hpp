#pragma once

#include <scene_set/scene_registry.hpp>
#include <scene_set/scene.hpp>
#include <scene_serialization/scene_serialization.hpp>
#include <scene_serialization/serializable_scene_graph.hpp>

#include <nodec/resource_management/resource_registry.hpp>


namespace ResourceImporter {


namespace internal {

void ProcessSceneEntityNode(
    std::shared_ptr<scene_serialization::SerializableEntityNode> node,
    const scene_serialization::SceneSerialization& sceneSerialization,
    scene_set::SceneEntity parent,
    scene_set::Scene& scene)
{
    using namespace scene_set::components;
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
    scene_set::SceneEntity destEntity,
    nodec::resource_management::ResourceRegistry& resourceRegistry,
    scene_set::Scene& scene,
    const scene_serialization::SceneSerialization& sceneSerialization)
{
    using namespace nodec;
    using namespace scene_serialization;

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
