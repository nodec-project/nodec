#ifndef NODEC__ERROR_FORMATTER_HPP_
#define NODEC__ERROR_FORMATTER_HPP_

#include <nodec/formatter.hpp>

#include <typeinfo>
#include <string>

namespace nodec {
namespace error_formatter {

inline std::string with_type_file_line(const std::string& message, const char* type, const char* file, const size_t line) noexcept {
    return Formatter() << type << ": " << message << "\n"
        << "[File] " << file << "\n"
        << "[Line] " << line;
}

template<typename T>
inline std::string with_type_file_line(const std::string& message, const char* file, const size_t line) noexcept {
    return with_type_file_line(message, typeid(T).name(), file, line);
}


}
}



#endif