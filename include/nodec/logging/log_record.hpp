#ifndef NODEC__LOGGING__LOG_RECORD_HPP_
#define NODEC__LOGGING__LOG_RECORD_HPP_

#include <chrono>
#include <cstddef>
#include <string_view>

#include "level.hpp"

namespace nodec {
namespace logging {

struct LogRecord {
public:
    LogRecord(std::chrono::system_clock::time_point time,
              std::string_view name,
              Level level,
              std::string_view message,
              std::string_view file, std::size_t line)
        : time(time), name(name), level(level), message(message), file(file), line(line) {}

    const std::chrono::system_clock::time_point time;
    std::string_view name;
    const Level level;
    std::string_view message;
    std::string_view file;
    const std::size_t line;
};

} // namespace logging
} // namespace nodec
#endif