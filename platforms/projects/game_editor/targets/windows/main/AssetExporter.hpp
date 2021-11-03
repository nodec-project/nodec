#pragma once

#include <assimp/scene.h>

#define CEREAL_THREAD_SAFE 1
#include <cereal/cereal.hpp>
#include <cereal/archives/xml.hpp>

#include <scene_set/scene_registry.hpp>
#include <scene_set/scene.hpp>
#include <rendering/components/mesh_renderer.hpp>
#include <nodec/resource_management/resource_registry.hpp>

#include <fstream>


// * <https://learnopengl.com/Model-Loading/Model>

namespace AssetExporter {


struct ResourceNameEntry {
    std::string source;
    std::string target;
};

using ResourceNameMap = std::unordered_map<std::string, ResourceNameEntry>;

namespace internal {
inline void ProcessNode(aiNode* pNode, const aiScene* pScene, const ResourceNameMap& nameMap,
                        scene_set::SceneEntity parentEntity,
                        scene_set::Scene& destScene,
                        const nodec::resource_management::ResourceRegistry& resourceRegistry) {
    using namespace nodec;
    using namespace nodec::entities;
    using namespace rendering::components;

    auto myEntity = destScene.create_entity(pNode->mName.C_Str());


    if (parentEntity != null_entity) {
        destScene.append_child(parentEntity, myEntity);
    }

    for (unsigned int i = 0; i < pNode->mNumMeshes; ++i) {
        auto meshIndex = pNode->mMeshes[i];
        auto mesh = pScene->mMeshes[meshIndex];

    }

    for (unsigned int i = 0; i < pNode->mNumChildren; ++i) {
        ProcessNode(pNode->mChildren[i], pScene, nameMap, myEntity, destScene, resourceRegistry);
    }
}

}


inline bool ExportMesh(aiMesh* pMesh, const std::string& destPath) {
    std::ofstream out(destPath);

    if (!out) {
        return false;
    }

    cereal::XMLOutputArchive archive(out);

    for (unsigned int i = 0; i < pMesh->mNumVertices; ++i) {

    }

    for (unsigned int i = 0; i < pMesh->mNumFaces; ++i) {

    }

    return true;
}

inline bool ExportMaterial(aiMaterial* pMaterial, const std::string& destPath) {

}

inline void ExportScene(const aiScene* pScene, scene_set::Scene& destScene, const ResourceNameMap& nameMap,
                        const nodec::resource_management::ResourceRegistry& resourceRegistry) {
    using namespace nodec::entities;

    internal::ProcessNode(pScene->mRootNode, pScene, nameMap, null_entity, destScene, resourceRegistry);

}


}