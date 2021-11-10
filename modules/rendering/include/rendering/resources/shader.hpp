#ifndef RENDERING__RESOURCES__SHADER_HPP_
#define RENDERING__RESOURCES__SHADER_HPP_

#include <string>

namespace rendering {
namespace resources {

class Shader {
public:
    Shader(const std::string& name)
        : name_{ name } {
    }

    ~Shader() {}

    const char* name() const noexcept {
        return name_.c_str();
    }

private:
    std::string name_;
};

}
}


#endif