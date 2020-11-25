#ifndef NODEC__NODEC_EXCEPTION_HPP_
#define NODEC__NODEC_EXCEPTION_HPP_

#include <exception>
#include <string>

namespace nodec
{

    class NodecException : public std::exception
    {
    public:
        NodecException(std::string message, const char* file, size_t line) noexcept;
        NodecException(const char* file, size_t line) noexcept;
        const char* what() const noexcept final;

    protected:
        std::string type = "NodecException";
        std::string file;
        size_t line;
        std::string message;

    private:
        mutable std::string what_buffer;
    };
}


#endif
