#ifndef NODEC__LOGGING__SIMPLE_FORMATTER_HPP_
#define NODEC__LOGGING__SIMPLE_FORMATTER_HPP_

#include <sstream>

#include "../log_record.hpp"

namespace nodec {
namespace logging {
namespace formatters {

struct SimpleFormatter {
    std::string operator()(const LogRecord &record) {
        std::ostringstream oss;

        switch (record.level) {
        case nodec::logging::Level::Unset:
            oss << "[UNSET]";
            break;
        case nodec::logging::Level::Debug:
            oss << "[DEBUG]";
            break;
        case nodec::logging::Level::Info:
            oss << "[INFO] ";
            break;
        case nodec::logging::Level::Warn:
            oss << "[WARN] ";
            break;
        case nodec::logging::Level::Error:
            oss << "[ERROR]";
            break;
        case nodec::logging::Level::Fatal:
            oss << "[FATAL]";
            break;
        default:
            oss << "[???]  ";
            break;
        }
        if (!record.name.empty()) {
            oss << " [" << record.name << "]";
        }

        oss << " - " << record.message << "\n";
        oss << "(" << record.file << " line " << record.line << ")\n";

        return oss.str();
    }
};

} // namespace formatters
} // namespace logging
} // namespace nodec
#endif