#ifndef NODEC_RENDERING__RESOURCES__SHADER_HPP_
#define NODEC_RENDERING__RESOURCES__SHADER_HPP_

#include <nodec/vector4.hpp>

#include <string>
#include <unordered_map>

namespace nodec_rendering {
namespace resources {

struct ShaderMetaInfo {
    struct FloatProperty {
        std::string name;
        float default_value;
    };

    struct Vector4Property {
        std::string name;
        nodec::Vector4f default_value;
    };

    struct TextureEntry {
        std::string name;
    };

    std::vector<FloatProperty> float_properties;
    std::vector<Vector4Property> vector4_properties;
    std::vector<TextureEntry> texture_entries;

    std::vector<std::string> pass;
    int render_priority{0};
};

struct SubShaderMetaInfo {
    std::vector<std::string> render_targets;
    std::vector<std::string> texture_resources;
};

class Shader {
};

} // namespace resources
} // namespace nodec_rendering

#endif