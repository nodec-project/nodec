#ifndef NODEC__NODEC_EXCEPTION_HPP_
#define NODEC__NODEC_EXCEPTION_HPP_

#include <exception>
#include <string>
#include <sstream>

namespace nodec
{

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
class NodecException : public std::exception
{
public:

    NodecException(std::string message, const char* file, size_t line) noexcept;
    NodecException(const char* file, size_t line) noexcept;

    const char* what() const noexcept final;
    const char* type() const noexcept;


    virtual ~NodecException();

protected:
    std::string file;
    size_t line;
    //std::ostringstream message;
    std::string message;

private:
    mutable const char* type_;
    mutable std::string what_buffer;
};

}


#endif
