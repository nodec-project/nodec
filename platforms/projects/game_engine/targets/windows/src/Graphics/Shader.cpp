#include "Graphics/Shader.hpp"

#include <fstream>

Shader::Shader(const std::string& path):
    fail_(false)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        fail_ = true;
        return;
    }

    bytecode = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});

}

bool Shader::Fail() const noexcept { return fail_; }