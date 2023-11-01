#ifndef NODEC__LOGGING__LEVEL_HPP_
#define NODEC__LOGGING__LEVEL_HPP_

#include <ostream>

namespace nodec {
namespace logging {

enum class Level {
    Unset = 0,  //! The unset log level
    Debug = 10, //! The debug log level
    Info = 20,  //! The info log level
    Warn = 30,  //! The warn log level
    Error = 40, //! The error log level
    Fatal = 50  //! The fatal log level
};

inline std::ostream &operator<<(std::ostream &stream, const nodec::logging::Level &level) {
    switch (level) {
    case nodec::logging::Level::Unset:
        return stream << "Unset";
    case nodec::logging::Level::Debug:
        return stream << "Debug";
    case nodec::logging::Level::Info:
        return stream << "Info";
    case nodec::logging::Level::Warn:
        return stream << "Warn";
    case nodec::logging::Level::Error:
        return stream << "Error";
    case nodec::logging::Level::Fatal:
        return stream << "Fatal";
    }
    return stream << "Unknown";
}

} // namespace logging
} // namespace nodec
#endif