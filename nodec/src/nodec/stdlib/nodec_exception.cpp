#include <sstream>

#include <nodec/stdlib/nodec_exception.hpp>

namespace nodec {
    namespace stdlib {
        NodecException::NodecException(const char* file, size_t line) noexcept
            :
            file(file),
            line(line)
        {}

        NodecException::NodecException(std::string message, const char* file, size_t line) noexcept
            :
            message(message),
            file(file),
            line(line)
        {}

        const char* NodecException::what() const noexcept {
            if (!what_buffer.empty())
            {
                return what_buffer.c_str();
            }

            std::ostringstream oss;
            oss << type << ":" << message << std::endl
                << "[File] " << file << std::endl
                << "[Line] " << line;
            what_buffer = oss.str();
            return what_buffer.c_str();
        }
    }

}