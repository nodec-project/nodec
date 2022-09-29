#pragma once

#include <cereal/cereal.hpp>

#include <string>

struct EditorConfig {
    std::string resource_path;

    struct FontConfig {
        std::string path;
        float pixel_size{10};

        template<class Archive>
        void serialize(Archive &archive) {
            archive(cereal::make_nvp("path", path),
                    cereal::make_nvp("pixel_size", pixel_size));
        }
    };

    FontConfig font;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("resource_path", resource_path),
                cereal::make_nvp("font", font));
    }
};
