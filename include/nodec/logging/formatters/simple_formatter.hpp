#ifndef NODEC__LOGGING__SIMPLE_FORMATTER_HPP_
#define NODEC__LOGGING__SIMPLE_FORMATTER_HPP_

#include <sstream>

#include "../../string_builder.hpp"
#include "../log_record.hpp"

namespace nodec {
namespace logging {
namespace formatters {

struct SimpleFormatter {
    std::string operator()(const LogRecord &record) {
        std::string text;
        StringBuilder builder(text);

        // std::ostringstream builder;

        switch (record.level) {
        case nodec::logging::Level::Unset:
            builder << "[UNSET]";
            break;
        case nodec::logging::Level::Debug:
            builder << "[DEBUG]";
            break;
        case nodec::logging::Level::Info:
            builder << "[INFO] ";
            break;
        case nodec::logging::Level::Warn:
            builder << "[WARN] ";
            break;
        case nodec::logging::Level::Error:
            builder << "[ERROR]";
            break;
        case nodec::logging::Level::Fatal:
            builder << "[FATAL]";
            break;
        default:
            builder << "[???]  ";
            break;
        }

        if (!record.name.empty()) {
            builder << " [" << record.name << "]";
        }

        builder << " - " << record.message << "\n";
        builder << "(" << record.file << " line " << record.line << ")\n";

        return std::move(text);
        // return builder.str();
    }
};

} // namespace formatters
} // namespace logging
} // namespace nodec
#endif