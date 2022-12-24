#include "ResourceExporter.hpp"

namespace {

std::unique_ptr<nodec_scene_serialization::SerializableEntityNode> ProcessSceneEntityNode(
    nodec_scene::SceneEntity entity,
    const nodec_scene::SceneRegistry &sceneRegistry,
    const nodec_scene_serialization::SceneSerialization &sceneSerialization) {
    using namespace nodec::entities;
    using namespace nodec_scene::components;
    using namespace nodec_scene_serialization::components;

    auto node = sceneSerialization.make_serializable_node(entity, sceneRegistry);

    auto *nonSerialized = sceneRegistry.try_get_component<NonSerialized>(entity);
    if (nonSerialized) return {};

    auto &hierarchy = sceneRegistry.get_component<Hierarchy>(entity);

    auto childEntity = hierarchy.first;
    while (childEntity != null_entity) {
        auto child = ProcessSceneEntityNode(childEntity, sceneRegistry, sceneSerialization);
        if (!child) continue;
        node->children.push_back(std::move(child));
        childEntity = sceneRegistry.get_component<Hierarchy>(childEntity).next;
    }

    return node;
}

} // namespace

bool ResourceExporter::ExportSceneGraph(const std::vector<nodec_scene::SceneEntity> &roots, const nodec_scene::SceneRegistry &sceneRegistry, const nodec_scene_serialization::SceneSerialization &sceneSerialization, nodec::resource_management::ResourceRegistry &resourceRegistry, const std::string &destPath) {
    using namespace nodec;
    using namespace nodec_scene_serialization;

    std::ofstream out(destPath, std::ios::binary);

    if (!out) return false;

    ArchiveContext context{resourceRegistry};

    using Options = cereal::JSONOutputArchive::Options;

    // cereal::UserDataAdapter<ArchiveContext, cereal::JSONOutputArchive> archive(context, out, Options(324, Options::IndentChar::space, 2u));
    cereal::UserDataAdapter<ArchiveContext, cereal::JSONOutputArchive> archive(context, out, Options::NoIndent());

    SerializableSceneGraph sceneGraph;

    for (auto &root : roots) {
        auto node = ProcessSceneEntityNode(root, sceneRegistry, sceneSerialization);
        if (!node) continue;
        sceneGraph.roots.push_back(std::move(node));
    }

    archive(cereal::make_nvp("scene_graph", sceneGraph));

    return true;
}
