#include <nodec/nodec_exception.hpp>

#include <sstream>
#include <typeinfo>

namespace nodec
{

NodecException::NodecException(std::string message, const char* file, size_t line) noexcept
    :
    message(message),
    file(file),
    line(line),
    type_(nullptr)
{
}

NodecException::NodecException(const char* file, size_t line) noexcept
    :
    file(file),
    line(line),
    type_(nullptr)
{
}

NodecException::~NodecException()
{
}

/**
* @note
*   <https://stackoverflow.com/questions/6747089/programmatically-getting-the-name-of-a-derived-class>
*/
const char* NodecException::type() const noexcept
{
    if (type_ == nullptr)
    {
        type_ = typeid(*this).name();
    }

    return type_;
}

const char* NodecException::what() const noexcept
{
    if (!what_buffer.empty())
    {
        return what_buffer.c_str();
    }

    std::ostringstream oss;
    oss << type() << ": " << message << std::endl
        << "[File] " << file << std::endl
        << "[Line] " << line;
    what_buffer = oss.str();
    return what_buffer.c_str();
}
}

