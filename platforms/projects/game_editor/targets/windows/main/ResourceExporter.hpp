#pragma once

#include <assimp/scene.h>

#define CEREAL_THREAD_SAFE 1
#include <cereal/cereal.hpp>
#include <cereal/archives/xml.hpp>

#include <fstream>

namespace ResourceExporter {

bool ExportMesh(aiMesh* pMesh, const std::string& destPath) {
    std::ofstream out(destPath);

    if (!out) {
        return false;
    }

    cereal::XMLOutputArchive archive(out);

    return true;
}



}