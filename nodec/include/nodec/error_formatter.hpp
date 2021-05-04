#ifndef NODEC__ERROR_FORMATTER_HPP_
#define NODEC__ERROR_FORMATTER_HPP_

#include <nodec/formatter.hpp>

#include <typeinfo>
#include <string>

namespace nodec {
namespace error_fomatter {

template<typename T>
std::string type_file_line(const std::string& message, const char* file, const size_t line) noexcept {
    return Formatter() << typeid(T).name() << ": " << message << "\n"
        << "[File] " << file << "\n"
        << "[Line] " << line;
}

}
}



#endif