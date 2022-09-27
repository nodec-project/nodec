#pragma once

#include <cereal/cereal.hpp>

#include <string>

struct EditorConfig {
    std::string resource_path;
};

template<class Archive>
void serialize(Archive& archive, EditorConfig &config) {
    archive(cereal::make_nvp("resource_path", config.resource_path));
}
