#ifndef NODEC__LOGGING__LOG_RECORD_HPP_
#define NODEC__LOGGING__LOG_RECORD_HPP_

#include <cstddef>
#include <string>

#include "level.hpp"

namespace nodec {
namespace logging {

struct LogRecord {
    LogRecord(const std::string &name, Level level, const std::string &message, const char *file, std::size_t line)
        : name(name), level(level), message(message), file(file), line(line) {}

    const std::string &name;
    Level level;
    const std::string &message;
    const char *file;
    std::size_t line;
};

} // namespace logging
} // namespace nodec
#endif