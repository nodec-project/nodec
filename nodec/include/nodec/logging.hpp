#ifndef NODEC__LOGGING_HPP_
#define NODEC__LOGGING_HPP_

#include <nodec/signals/signal.hpp>

#include <functional>
#include <ostream>
#include <string>

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

struct LogRecord {
    LogRecord(Level level, const std::string &message, const char *file, size_t line);
    Level level;
    std::string message;
    const char *file;
    size_t line;
};

using RecordHandlers = signals::Signal<void(const LogRecord &)>;

extern std::function<std::string(const LogRecord &)> formatter;

std::string default_formatter(const LogRecord &record) noexcept;

/**
 * @brief Record handlers can be connected through this interface.
 *
 *   Example:
 *       // connect stdout handler.
 *       record_handlers().connect(logging::record_to_stdout_handler);
 * 
 * @todo Support thread-safe.
 */
RecordHandlers::SignalInterface record_handlers();

/**
 * @brief Output the log record to stdout.
 */
void record_to_stdout_handler(const LogRecord &record) noexcept;

/**
 * @brief Sets the threshold for this logger to level.
 *   Logging messages which are less severe than level will be ignored;
 *   logging messages which have severity level or higher will be emitted
 *   by whichever handler or handlers service this logger
 */
void set_level(Level level);

/**
 * @detail
 *   DEBUG: Detailed information, typically of interest only when diagnosing problems.
 */
void debug(const std::string &message, const char *file, size_t line);

/**
 * @detail
 *   INFO: Confirmation that things are working as expected.
 */
void info(const std::string &message, const char *file, size_t line);

/**
 * @detail
 *   WARNING: An indication that something unexpected happened, or indicative of some problem in the near future (e.g. 'disk space low').
 *       The software is still working as expected.
 */
void warn(const std::string &message, const char *file, size_t line);

/**
 * @detail
 *   ERROR: Due to a more serious problem, the software has not been able to perform some function.
 */
void error(const std::string &message, const char *file, size_t line);

/**
 * @detail
 *   FATAL: A serious error, indicating that the program itself may be unable to continue running.
 */
void fatal(const std::string &message, const char *file, size_t line);

/**
 * @brief Logs a message with level.
 */
void log(Level level, const std::string &message, const char *file, size_t line);

namespace details {
class LogStreamBufferGeneric;
}

class LogStream : public std::ostream {
public:
    LogStream(Level level, const char *file, size_t line);
    ~LogStream();

private:
    details::LogStreamBufferGeneric *buffer;
};

class DebugStream : public LogStream {
public:
    DebugStream(const char *file, size_t line);
};

class InfoStream : public LogStream {
public:
    InfoStream(const char *file, size_t line);
};

class WarnStream : public LogStream {
public:
    WarnStream(const char *file, size_t line);
};

class ErrorStream : public LogStream {
public:
    ErrorStream(const char *file, size_t line);
};

class FatalStream : public LogStream {
public:
    FatalStream(const char *file, size_t line);
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

inline std::ostream &operator<<(std::ostream &stream, const nodec::logging::LogRecord &record) {
    return stream << nodec::logging::formatter(record);
}

} // namespace logging
} // namespace nodec

#endif