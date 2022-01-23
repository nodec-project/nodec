#ifndef NODEC__FORMATTER_HPP_
#define NODEC__FORMATTER_HPP_

#include <nodec/macros.hpp>

#include <sstream>
#include <typeinfo>

namespace nodec {

/*
* The Text Formatter
* 
* if you use C++20, consider to use std::format.
*   * <https://en.cppreference.com/w/cpp/utility/format>
* 
* It is designed for easy use of text format even though under C++20.
* 
* Implementation refs:
*   * <https://stackoverflow.com/questions/12261915/how-to-throw-stdexceptions-with-variable-messages>
* 
*/

class Formatter {
public:
    Formatter() noexcept {}
    ~Formatter() noexcept {}

    template<typename T>
    Formatter& operator<<(const T& value) noexcept {
        stream_ << value;
        return *this;
    }

    operator std::string() const noexcept {
        return stream_.str();
    }

private:
    std::ostringstream stream_;

    NODEC_DISABLE_COPY(Formatter);
};


template<typename T>
class ErrorFormatter {
public:
    ErrorFormatter(const char* file, const size_t line) noexcept
        : type_{ typeid(T).name() }
        , file_{ file }
        , line_{ line } {
        stream_ << type_ << ": ";
    }

    template<typename T>
    ErrorFormatter& operator<<(const T& value) noexcept {
        stream_ << value;
        return *this;
    }

    operator std::string() noexcept {
        stream_ << "\n"
            "[File] " << file_ << "\n"
            "[Line] " << line_;
        return stream_.str();
    }

private:
    std::ostringstream stream_;

    const char* type_;
    const char* file_;
    const size_t line_;

    NODEC_DISABLE_COPY(ErrorFormatter);
};

}


#endif