#include <nodec/logging.hpp>

#include <iostream>
#include <mutex>
#include <sstream>

namespace nodec {
namespace logging {

namespace {
Level current_level_ = Level::Debug;
RecordHandlers record_handlers_;
} // namespace

std::function<std::string(const LogRecord &)> formatter = default_formatter;

LogRecord::LogRecord(Level level, const std::string &message, const char *file, size_t line)
    : level(level),
      message(message),
      file(file),
      line(line) {
}

void set_level(Level level) {
    current_level_ = level;
}

std::string default_formatter(const LogRecord &record) noexcept {
    std::ostringstream oss;
    oss << "[" << record.file << " line " << record.line << "]\n";
    switch (record.level) {
    case nodec::logging::Level::Unset:
        oss << "UNSET  : ";
        break;
    case nodec::logging::Level::Debug:
        oss << "DEBUG  : ";
        break;
    case nodec::logging::Level::Info:
        oss << "INFO   : ";
        break;
    case nodec::logging::Level::Warn:
        oss << "WARNING: ";
        break;
    case nodec::logging::Level::Error:
        oss << "ERROR  : ";
        break;
    case nodec::logging::Level::Fatal:
        oss << "FATAL  : ";
        break;
    default:
        oss << "UNKOWN : ";
        break;
    }

    oss << record.message << "\n";

    return oss.str();
}

void record_to_stdout_handler(const LogRecord &record) noexcept {
    std::cout << record << std::endl;
}

RecordHandlers::SignalInterface record_handlers() {
    return record_handlers_.signal_interface();
}

namespace {
void log_generic(const LogRecord &record) {
    // Wait until one record handled.
    static std::mutex io_lock_mtx_;
    std::lock_guard<std::mutex> lock(io_lock_mtx_);

    if (record.level < current_level_) {
        // ignore this log
        return;
    }

    // Handler objects are dispatching the appropriate log messages (based on the log messages' severity)
    // to the handler's specified destination.
    record_handlers_(record);
}
} // namespace

void debug(const std::string &message, const char *file, size_t line) {
    log_generic(LogRecord{Level::Debug, message, file, line});
}

void info(const std::string &message, const char *file, size_t line) {
    log_generic(LogRecord{Level::Info, message, file, line});
}

void warn(const std::string &message, const char *file, size_t line) {
    log_generic(LogRecord{Level::Warn, message, file, line});
}

void error(const std::string &message, const char *file, size_t line) {
    log_generic(LogRecord{Level::Error, message, file, line});
}

void fatal(const std::string &message, const char *file, size_t line) {
    log_generic(LogRecord{Level::Fatal, message, file, line});
}

void log(Level level, const std::string &message, const char *file, size_t line) {
    log_generic(LogRecord{level, message, file, line});
}

namespace details {
class LogStreamBufferGeneric : public std::streambuf {
    using Base = std::streambuf;

public:
    LogStreamBufferGeneric(const Level level, const char *file, size_t line)
        : level(level),
          file(file),
          line(line) {
    }

public:
    int overflow(int ch) override {
        message += ch;
        return ch;
    }
    int sync() override {
        if (!message.empty()) {
            log_generic(LogRecord{level, message, file, line});
            message.clear();
        }
        return 0; // not -1: means it ok.
    }

private:
    std::string message;
    Level level;
    const char *file;
    size_t line;
};
} // namespace details

LogStream::LogStream(Level level, const char *file, size_t line)
    : std::ostream(buffer = new details::LogStreamBufferGeneric(level, file, line)) {}

LogStream::~LogStream() {
    buffer->sync();
    delete buffer;
}

DebugStream::DebugStream(const char *file, size_t line)
    : LogStream(Level::Debug, file, line) {
}

InfoStream::InfoStream(const char *file, size_t line)
    : LogStream(Level::Info, file, line) {
}

WarnStream::WarnStream(const char *file, size_t line)
    : LogStream(Level::Warn, file, line) {
}

ErrorStream::ErrorStream(const char *file, size_t line)
    : LogStream(Level::Error, file, line) {
}

FatalStream::FatalStream(const char *file, size_t line)
    : LogStream(Level::Fatal, file, line) {
}

} // namespace logging
} // namespace nodec

