#ifndef NODEC__EXCEPTION_HPP_
#define NODEC__EXCEPTION_HPP_

#include <exception>
#include <string>
#include <sstream>
#include <typeinfo>

namespace nodec {

/**
* @note
*   This exception class not inhereited from std::exception, because no merit.
*   std::exception is desugned under NOT wide charcter.
*   <https://stackoverflow.com/questions/38186579/unicode-exception-class>
*
*   ^ 2020-12-24:
*       We decided to inherit std::exception.
*       We're considering that this class can be caught by std::exception type.
*
*   wchar_t is not clearly defined, depended on platform.
*   so we should not use it.
*   <https://stackoverflow.com/questions/50403342/how-do-i-properly-use-stdstring-on-utf-8-in-c>
*/
class Exception : public std::exception {
public:

    Exception(std::string message, const char* file, size_t line) noexcept
        :file(file), line(line), message(message), type_(nullptr) {
    }

    Exception(const char* file, size_t line) noexcept
        :file(file), line(line), type_(nullptr) {
    }

    const char* what() const noexcept final {
        if (!what_.empty()) {
            return what_.c_str();
        }

        std::ostringstream oss;
        oss << type() << ": " << message << std::endl
            << "[File] " << file << std::endl
            << "[Line] " << line;
        what_ = oss.str();
        return what_.c_str();
    }

    /**
    * @note
    *   <https://stackoverflow.com/questions/6747089/programmatically-getting-the-name-of-a-derived-class>
    */
    const char* type() const noexcept {
        if (!type_) {
            type_ = typeid(*this).name();
        }

        return type_;
    }


    virtual ~Exception() {}

protected:
    const char* const file;
    const size_t line;
    std::string message;

private:
    mutable const char* type_;
    mutable std::string what_;
};

}


#endif
