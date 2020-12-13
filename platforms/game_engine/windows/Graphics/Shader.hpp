#pragma once

#include <nodec/macros.hpp>
#include <string>
#include <vector>

class Shader
{
public:
    Shader(const std::string& path);

    bool Fail() const noexcept;

protected:
    std::vector<uint8_t> bytecode;

private:
    bool fail_;

private:
    NODEC_DISABLE_COPY(Shader);
};