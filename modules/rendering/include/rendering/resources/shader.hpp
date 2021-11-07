#ifndef RENDERING__RESOURCES__SHADER_HPP_
#define RENDERING__RESOURCES__SHADER_HPP_

#include <string>

namespace rendering {
namespace resources {

class Shader {
public:
    Shader(const std::string& shader_name)
        : shader_name_{ shader_name } {
    }

    ~Shader() {}

    const char* shader_name() const noexcept {
        return shader_name_.c_str();
    }

private:
    std::string shader_name_;
};

}
}


#endif