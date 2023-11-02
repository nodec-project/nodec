#ifndef NODEC__LOGGING__LOGGING_HPP_
#define NODEC__LOGGING__LOGGING_HPP_

#include "logger.hpp"
#include "logger_repository.hpp"

namespace nodec {
namespace logging {

class LogStream {
public:
    LogStream(std::shared_ptr<Logger> logger, Level level, const char *file, std::size_t line)
        : logger_(logger), level_(level), file_(file), line_(line) {}

    ~LogStream() {
        logger_->log(level_, stream_.str(), file_, line_);
    }

    template<typename T>
    LogStream &operator<<(const T &value) {
        stream_ << value;
        return *this;
    }

private:
    std::shared_ptr<Logger> logger_;
    Level level_;
    const char *file_;
    std::size_t line_;
    std::ostringstream stream_;
};

inline std::shared_ptr<Logger> get_logger(const std::string &name = "") {
    return LoggerRepositoryLocater::get().get_logger(name);
}

inline void debug(const std::string &message, const char *file, std::size_t line) {
    LoggerRepositoryLocater::get().root_logger()->debug(message, file, line);
}

inline void info(const std::string &message, const char *file, std::size_t line) {
    LoggerRepositoryLocater::get().root_logger()->info(message, file, line);
}

inline void warn(const std::string &message, const char *file, std::size_t line) {
    LoggerRepositoryLocater::get().root_logger()->warn(message, file, line);
}

inline void error(const std::string &message, const char *file, std::size_t line) {
    LoggerRepositoryLocater::get().root_logger()->error(message, file, line);
}

inline void fatal(const std::string &message, const char *file, std::size_t line) {
    LoggerRepositoryLocater::get().root_logger()->fatal(message, file, line);
}

inline void log(Level level, const std::string &message, const char *file, std::size_t line) {
    LoggerRepositoryLocater::get().root_logger()->log(level, message, file, line);
}

inline LogStream debug(const char *file, std::size_t line) {
    return {LoggerRepositoryLocater::get().root_logger(), Level::Debug, file, line};
}

inline LogStream info(const char *file, std::size_t line) {
    return {LoggerRepositoryLocater::get().root_logger(), Level::Info, file, line};
}

inline LogStream warn(const char *file, std::size_t line) {
    return {LoggerRepositoryLocater::get().root_logger(), Level::Warn, file, line};
}

inline LogStream error(const char *file, std::size_t line) {
    return {LoggerRepositoryLocater::get().root_logger(), Level::Error, file, line};
}

inline LogStream fatal(const char *file, std::size_t line) {
    return {LoggerRepositoryLocater::get().root_logger(), Level::Fatal, file, line};
}

inline LogStream log(Level level, const char *file, std::size_t line) {
    return {LoggerRepositoryLocater::get().root_logger(), level, file, line};
}
} // namespace logging
} // namespace nodec

#endif