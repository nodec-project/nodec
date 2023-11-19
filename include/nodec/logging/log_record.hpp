#ifndef NODEC__LOGGING__LOG_RECORD_HPP_
#define NODEC__LOGGING__LOG_RECORD_HPP_

#include <chrono>
#include <cstddef>
#include <string>

#include "level.hpp"

namespace nodec {
namespace logging {

struct LogRecord {
public:
    LogRecord(std::chrono::system_clock::time_point time, const std::string &name, Level level, const std::string &message, const char *file, std::size_t line)
        : time(time), name(name), level(level), message(message), file(file), line(line) {}

    const std::chrono::system_clock::time_point time;
    const std::string &name;
    const Level level;
    const std::string &message;
    const char *file;
    const std::size_t line;
};

} // namespace logging
} // namespace nodec
#endif