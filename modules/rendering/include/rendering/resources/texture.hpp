#ifndef RENDERING__RESOURCES__TEXTURE_HPP_
#define RENDERING__RESOURCES__TEXTURE_HPP_

#include <string>

namespace rendering {
namespace resources {

class Texture {

public:
    Texture(const std::string& name)
        : name_{ name } {
    }

    ~Texture() {}

    const char* name() const noexcept {
        return name_.c_str();
    }

private:
    std::string name_;
};

}
}

#endif