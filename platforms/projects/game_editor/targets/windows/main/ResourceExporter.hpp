#pragma once

#define CEREAL_THREAD_SAFE 1

#include <SceneSerialization/SerializableBasicComponents.hpp>

#include <scene_set/scene_registry.hpp>
#include <scene_set/scene.hpp>
#include <rendering/components/mesh_renderer.hpp>
#include <rendering/resources/mesh.hpp>
#include <rendering/resources/material.hpp>
#include <serialization/rendering/resources/mesh.hpp>
#include <serialization/rendering/resources/material.hpp>
#include <scene_serialization/scene_serialization.hpp>

#include <nodec/resource_management/resource_registry.hpp>

#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <fstream>


// * <https://learnopengl.com/Model-Loading/Model>

namespace ResourceExporter {


struct ResourceNameEntry {
    std::string source;
    std::string target;
};

using ResourceNameMap = std::unordered_map<std::string, ResourceNameEntry>;

namespace internal {
inline void ProcessNode(aiNode* pNode, const aiScene* pScene, const ResourceNameMap& nameMap,
                        scene_set::SceneEntity parentEntity,
                        scene_set::Scene& destScene,
                        nodec::resource_management::ResourceRegistry& resourceRegistry) {
    using namespace nodec;
    using namespace nodec::entities;
    using namespace rendering::components;
    using namespace rendering::resources;
    using namespace scene_set::components;

    auto myEntity = destScene.create_entity(pNode->mName.C_Str());

    {
        aiVector3D position;
        aiQuaternion rotation;
        aiVector3D scale;
        pNode->mTransformation.Decompose(scale, rotation, position);

        auto& trfm = destScene.registry().get_component<Transform>(myEntity);
        trfm.local_position.set(position.x, position.y, position.z);
        trfm.local_rotation.set(rotation.x, rotation.y, rotation.z, rotation.w);
        trfm.local_scale.set(scale.x, scale.y, scale.z);
        trfm.dirty = true;
    }

    if (parentEntity != null_entity) {
        destScene.append_child(parentEntity, myEntity);
    }

    //auto& name = destScene.registry().get_component<Name>(myEntity);

    if (pNode->mNumMeshes > 0) {
        destScene.registry().emplace_component<MeshRenderer>(myEntity);
    }

    auto* meshRenderer = destScene.registry().try_get_component<MeshRenderer>(myEntity);

    for (unsigned int i = 0; i < pNode->mNumMeshes; ++i) {
        assert(meshRenderer);

        auto meshIndex = pNode->mMeshes[i];
        auto materialIndex = pScene->mMeshes[meshIndex]->mMaterialIndex;

        auto &meshPath = nameMap.at(Formatter() << "mesh-" << meshIndex).target;
        auto mesh = resourceRegistry.get_resource<Mesh>(meshPath).get();
        meshRenderer->meshes.push_back(mesh);
        
        auto& materialPath = nameMap.at(Formatter() << "material-" << materialIndex).target;
        auto material = resourceRegistry.get_resource<Material>(materialPath).get();
        meshRenderer->materials.push_back(material);
    }


    for (unsigned int i = 0; i < pNode->mNumChildren; ++i) {
        ProcessNode(pNode->mChildren[i], pScene, nameMap, myEntity, destScene, resourceRegistry);
    }
}

}


inline bool ExportMesh(aiMesh* pMesh, const std::string& destPath) {
    using namespace nodec;
    using namespace rendering::resources;

    std::ofstream out(destPath, std::ios::binary);

    if (!out) {
        return false;
    }
    
    //cereal::JSONOutputArchive archive(out);
    cereal::PortableBinaryOutputArchive archive(out);

    SerializableMesh mesh;

    for (unsigned int i = 0; i < pMesh->mNumVertices; ++i) {
        auto &position = pMesh->mVertices[i];
        auto& normal = pMesh->mNormals[i];


        SerializableMesh::Vertex vert;
        vert.position.set(position.x, position.y, position.z);
        vert.normal.set(normal.x, normal.y, normal.z);


        if (pMesh->mTextureCoords[0]) {
            auto& uv = pMesh->mTextureCoords[0][i];
            vert.uv.set(uv.x, uv.y);
        }

        if (pMesh->mTangents) {
            auto& tangent = pMesh->mTangents[i];
            vert.tangent.set(tangent.x, tangent.y, tangent.z);
        }
        mesh.vertices.push_back(vert);
    }

    for (unsigned int i = 0; i < pMesh->mNumFaces; ++i) {
        // 3 indices per face
        auto& face = pMesh->mFaces[i];

        assert(face.mNumIndices == 3 && "Only 3 indices available. Make sure to set aiProcess_Triangulate on call Assimp::Importer::ReadFile.");
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            mesh.triangles.emplace_back(face.mIndices[j]);
        }
    }

    archive(cereal::make_nvp("mesh", mesh));

    return true;
}

inline bool ExportMaterial(aiMaterial* pMaterial, const std::string& destPath) {
    using namespace nodec;
    using namespace rendering::resources;

    std::ofstream out(destPath, std::ios::binary);

    if (!out) {
        return false;
    }

    cereal::JSONOutputArchive archive(out);

    SerializableMaterial material;
    //material.float_properties["albedo"];
    //material.float_properties["metalness"];
    //material.texture_properties["albedo"] = { std::make_shared<Texture>("albedo.tga"), Sampler::Bilinear };
    //material.vector4_properties["ao"];

    archive(cereal::make_nvp("material", material));

    return true;
}

inline void ExportScene(const aiScene* pScene, scene_set::Scene& destScene, const ResourceNameMap& nameMap,
                        nodec::resource_management::ResourceRegistry& resourceRegistry) {
    using namespace nodec::entities;

    internal::ProcessNode(pScene->mRootNode, pScene, nameMap, null_entity, destScene, resourceRegistry);
}


namespace internal {

inline std::shared_ptr<scene_serialization::SerializableEntityNode> ProcessSceneEntityNode(
    scene_set::SceneEntity entity,
    const scene_set::SceneRegistry& sceneRegistry,
    const scene_serialization::SceneSerialization& sceneSerializaton) 
{

    auto node = sceneSerializaton.make_serializable_node(entity, sceneRegistry);


    return node;
}

}

inline bool ExportSceneGraph(
    const std::vector<scene_set::SceneEntity>& roots,
    const scene_set::SceneRegistry& sceneRegistry,
    const scene_serialization::SceneSerialization& sceneSerialization,
    const std::string& destPath) {

    using namespace nodec;
    using namespace scene_serialization;

    std::ofstream out(destPath, std::ios::binary);

    if (!out) {
        return false;
    }

    cereal::JSONOutputArchive archive(out);


    SerializableSceneGraph sceneGraph;

    for (auto& root : roots) {
        auto node = internal::ProcessSceneEntityNode(root, sceneRegistry, sceneSerialization);
        sceneGraph.roots.push_back(node);
        
        
    }

    archive(cereal::make_nvp("scene_graph", sceneGraph));

    return true;
}


}