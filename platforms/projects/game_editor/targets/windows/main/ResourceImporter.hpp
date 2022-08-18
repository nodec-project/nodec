#pragma once

#include <nodec_scene/scene.hpp>
#include <nodec_scene/scene_registry.hpp>
#include <nodec_scene_serialization/scene_entity_emplacer.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>
#include <nodec_scene_serialization/serializable_scene_graph.hpp>

#include <nodec/resource_management/resource_registry.hpp>

namespace ResourceImporter {

bool ImportSceneGraph(
    const std::string &path,
    nodec_scene::SceneEntity destEntity,
    nodec::resource_management::ResourceRegistry &resourceRegistry,
    nodec_scene::Scene &scene,
    const nodec_scene_serialization::SceneSerialization &sceneSerialization) {
    using namespace nodec;
    using namespace nodec_scene_serialization;

    auto graph = resourceRegistry.get_resource<SerializableSceneGraph>(path).get();
    if (!graph) return false;

    SceneEntityEmplacer{graph, scene, destEntity, sceneSerialization}.emplace_all();

    return true;
}

} // namespace ResourceImporter
