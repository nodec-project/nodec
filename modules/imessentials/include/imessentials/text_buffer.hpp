#ifndef IMESSENTIALS__TEXT_BUFFER_HPP_
#define IMESSENTIALS__TEXT_BUFFER_HPP_

#include <string>
#include <vector>

namespace imessentials {

inline std::vector<char> &get_text_buffer(std::size_t buffer_size, const std::string &initial_text) {
    static std::vector<char> buffer;
    buffer.resize(buffer_size);

    initial_text.copy(buffer.data(), buffer.size() - 1);
    const auto null_pos = (std::min)(initial_text.size(), buffer.size() - 1);
    buffer[null_pos] = '\0';
    return buffer;
}

} // namespace imessentials

#endif