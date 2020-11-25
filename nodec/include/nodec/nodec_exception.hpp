#ifndef NODEC__NODEC_EXCEPTION_HPP_
#define NODEC__NODEC_EXCEPTION_HPP_

#include <exception>
#include <string>

namespace nodec
{
    /**
    * @note
    *   This exception class not inhereited from std::exception, because no merit.
    *   std::exception is desugned under NOT wide charcter.
    *   <https://stackoverflow.com/questions/38186579/unicode-exception-class>
    * 
    *   wchar_t is not clearly defined, depended on platform.
    *   so we should not use it.
    *   <https://stackoverflow.com/questions/50403342/how-do-i-properly-use-stdstring-on-utf-8-in-c>
    */
    class NodecException : std::exception
    {
    public:

        NodecException(std::string message, const char* file, size_t line) noexcept;
        NodecException(const char* file, size_t line) noexcept;
        
        const char* what() const noexcept final;
        virtual const char* type() const noexcept { return "NodecException"; }

    protected:
        std::string file;
        size_t line;
        std::string message;

    private:
        mutable std::string what_buffer;
    };
}


#endif
