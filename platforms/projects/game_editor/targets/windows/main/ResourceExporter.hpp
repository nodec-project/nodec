#pragma once

#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_rendering/resources/material.hpp>
#include <nodec_rendering/resources/mesh.hpp>
#include <nodec_scene/scene.hpp>
#include <nodec_scene/scene_registry.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>
#include <nodec_scene_serialization/serializable_scene_graph.hpp>
#include <nodec_serialization/nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_serialization/nodec_rendering/resources/material.hpp>
#include <nodec_serialization/nodec_rendering/resources/mesh.hpp>

#include <nodec/resource_management/resource_registry.hpp>

#include <assimp/matrix4x4.h>
#include <assimp/scene.h>

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/cereal.hpp>

#include <fstream>

// * <https://learnopengl.com/Model-Loading/Model>

namespace ResourceExporter {

struct ResourceNameEntry {
    std::string source;
    std::string target;
};

using ResourceNameMap = std::unordered_map<std::string, ResourceNameEntry>;

namespace internal {
inline void ProcessNode(
    aiNode *pNode, const aiScene *pScene,
    const std::string &resource_name_prefix,
    const ResourceNameMap &nameMap,
    nodec_scene::SceneEntity parentEntity,
    nodec_scene::Scene &destScene,
    nodec::resource_management::ResourceRegistry &resourceRegistry) {
    using namespace nodec;
    using namespace nodec::entities;
    using namespace nodec_rendering::components;
    using namespace nodec_rendering::resources;
    using namespace nodec_scene::components;

    auto myEntity = destScene.create_entity(pNode->mName.C_Str());

    {
        aiVector3D position;
        aiQuaternion rotation;
        aiVector3D scale;
        pNode->mTransformation.Decompose(scale, rotation, position);

        auto &trfm = destScene.registry().get_component<Transform>(myEntity);
        trfm.local_position.set(position.x, position.y, position.z);
        trfm.local_rotation.set(rotation.x, rotation.y, rotation.z, rotation.w);
        trfm.local_scale.set(scale.x, scale.y, scale.z);
        trfm.dirty = true;
    }

    if (parentEntity != null_entity) {
        destScene.hierarchy_system().append_child(parentEntity, myEntity);
    }

    // auto& name = destScene.registry().get_component<Name>(myEntity);

    if (pNode->mNumMeshes > 0) {
        destScene.registry().emplace_component<MeshRenderer>(myEntity);
    }

    auto *meshRenderer = destScene.registry().try_get_component<MeshRenderer>(myEntity);

    for (unsigned int i = 0; i < pNode->mNumMeshes; ++i) {
        assert(meshRenderer);

        auto meshIndex = pNode->mMeshes[i];
        auto materialIndex = pScene->mMeshes[meshIndex]->mMaterialIndex;

        std::string meshPath = Formatter() << resource_name_prefix << nameMap.at(Formatter() << "mesh-" << meshIndex).target;
        auto mesh = resourceRegistry.get_resource<Mesh>(meshPath).get();
        meshRenderer->meshes.push_back(mesh);

        std::string materialPath = Formatter() << resource_name_prefix << nameMap.at(Formatter() << "material-" << materialIndex).target;
        auto material = resourceRegistry.get_resource<Material>(materialPath).get();
        meshRenderer->materials.push_back(material);
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; ++i) {
        ProcessNode(pNode->mChildren[i], pScene, resource_name_prefix, nameMap, myEntity, destScene, resourceRegistry);
    }
}

} // namespace internal

inline bool ExportMesh(aiMesh *pMesh, const std::string &destPath) {
    using namespace nodec;
    using namespace nodec_rendering::resources;

    std::ofstream out(destPath, std::ios::binary);

    if (!out) {
        return false;
    }

    // cereal::JSONOutputArchive archive(out);
    cereal::PortableBinaryOutputArchive archive(out);

    SerializableMesh mesh;

    for (unsigned int i = 0; i < pMesh->mNumVertices; ++i) {
        auto &position = pMesh->mVertices[i];
        auto &normal = pMesh->mNormals[i];

        SerializableMesh::Vertex vert;
        vert.position.set(position.x, position.y, position.z);
        vert.normal.set(normal.x, normal.y, normal.z);

        if (pMesh->mTextureCoords[0]) {
            auto &uv = pMesh->mTextureCoords[0][i];
            vert.uv.set(uv.x, uv.y);
        }

        if (pMesh->mTangents) {
            auto &tangent = pMesh->mTangents[i];
            vert.tangent.set(tangent.x, tangent.y, tangent.z);
        }
        mesh.vertices.push_back(vert);
    }

    for (unsigned int i = 0; i < pMesh->mNumFaces; ++i) {
        // 3 indices per face
        auto &face = pMesh->mFaces[i];

        assert(face.mNumIndices == 3 && "Only 3 indices available. Make sure to set aiProcess_Triangulate on call Assimp::Importer::ReadFile.");
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            mesh.triangles.emplace_back(face.mIndices[j]);
        }
    }

    archive(cereal::make_nvp("mesh", mesh));

    return true;
}

inline bool ExportMaterial(aiMaterial *pMaterial, const std::string &destPath) {
    using namespace nodec;
    using namespace nodec_rendering::resources;

    std::ofstream out(destPath, std::ios::binary);

    if (!out) {
        return false;
    }

    cereal::JSONOutputArchive archive(out);

    SerializableMaterial material;
    material.shader = "org.nodec-rendering.essentials/shaders/pbr";
    // material.float_properties["albedo"];
    // material.float_properties["metalness"];
    // material.texture_properties["albedo"] = { "albedo.tga", Sampler::Bilinear };
    // material.vector4_properties["ao"];

    archive(cereal::make_nvp("material", material));

    return true;
}

inline void ExportScene(const aiScene *pScene, nodec_scene::Scene &destScene, const std::string &resource_name_prefix, const ResourceNameMap &nameMap,
                        nodec::resource_management::ResourceRegistry &resourceRegistry) {
    using namespace nodec::entities;

    internal::ProcessNode(pScene->mRootNode, pScene, resource_name_prefix, nameMap, null_entity, destScene, resourceRegistry);
}

namespace internal {

inline std::shared_ptr<nodec_scene_serialization::SerializableEntityNode> ProcessSceneEntityNode(
    nodec_scene::SceneEntity entity,
    const nodec_scene::SceneRegistry &sceneRegistry,
    const nodec_scene_serialization::SceneSerialization &sceneSerialization) {
    using namespace nodec::entities;
    using namespace nodec_scene::components;

    auto node = sceneSerialization.make_serializable_node(entity, sceneRegistry);

    auto &hierarchy = sceneRegistry.get_component<Hierarchy>(entity);

    auto childEntity = hierarchy.first;
    while (childEntity != null_entity) {
        auto child = ProcessSceneEntityNode(childEntity, sceneRegistry, sceneSerialization);
        node->children.push_back(child);
        childEntity = sceneRegistry.get_component<Hierarchy>(childEntity).next;
    }

    return node;
}

} // namespace internal

inline bool ExportSceneGraph(
    const std::vector<nodec_scene::SceneEntity> &roots,
    const nodec_scene::SceneRegistry &sceneRegistry,
    const nodec_scene_serialization::SceneSerialization &sceneSerialization,
    nodec::resource_management::ResourceRegistry &resourceRegistry,
    const std::string &destPath) {
    using namespace nodec;
    using namespace nodec_scene_serialization;

    std::ofstream out(destPath, std::ios::binary);

    if (!out) return false;

    ArchiveContext context{resourceRegistry};

    // Options options(324, Options::IndentChar::space, 2U);
    using Options = cereal::JSONOutputArchive::Options;
    cereal::UserDataAdapter<ArchiveContext, cereal::JSONOutputArchive> archive(context, out, Options::NoIndent());

    SerializableSceneGraph sceneGraph;

    for (auto &root : roots) {
        auto node = internal::ProcessSceneEntityNode(root, sceneRegistry, sceneSerialization);
        sceneGraph.roots.push_back(node);
    }

    archive(cereal::make_nvp("scene_graph", sceneGraph));

    return true;
}

} // namespace ResourceExporter
