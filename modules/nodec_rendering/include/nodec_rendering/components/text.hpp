#ifndef NODEC_RENDERING__COMPONENTS__TEXT_HPP_
#define NODEC_RENDERING__COMPONENTS__TEXT_HPP_

#include <string>

namespace nodec_rendering {
namespace components {

struct Text {
    std::string text;
    std::string font;
    int pixel_size{10};
};
}
} // namespace nodec_rendering

#endif