#ifndef NODEC__LOGGING__LOGGER_HPP_
#define NODEC__LOGGING__LOGGER_HPP_

#include <atomic>
#include <memory>
#include <mutex>
#include <sstream>
#include <vector>

#include "../macros.hpp"
#include "../optional.hpp"
#include "../signals/signal.hpp"
#include "log_record.hpp"

namespace nodec {
namespace logging {

class HandlerConnection {
public:
    HandlerConnection(signals::Connection &&connection, std::mutex &handlers_mutex)
        : conn_(std::move(connection)), handlers_mutex_(handlers_mutex) {}

    ~HandlerConnection() {
        std::lock_guard<std::mutex> lock(handlers_mutex_);
        conn_.reset();
    }

    HandlerConnection(HandlerConnection &&other)
        : conn_(std::move(other.conn_)), handlers_mutex_(std::move(other.handlers_mutex_)) {
    }

    /**
     * @brief Blocks the connection.
     * During blocking, the handler belongs to the connection won't be emitted.
     *
     * @warning
     * Use this function only during handler is emitted.
     */
    void block() {
        conn_->block();
    }

    void unblock() {
        conn_->unblock();
    }

private:
    optional<signals::Connection> conn_;
    std::mutex &handlers_mutex_;
    NODEC_DISABLE_COPY(HandlerConnection)
};

class Logger {
private:
    void handle(const LogRecord &record) {
        if (record.level < level_) return;
        {
            std::lock_guard<std::mutex> lock(handlers_mutex_);
            handlers_(record);
        }
        if (parent_) {
            parent_->handle(record);
        }
    }
    
    class LogStream {
    public:
        LogStream(Logger &logger, Level level, const char *file, std::size_t line)
            : logger_(logger), level_(level), file_(file), line_(line) {}

        ~LogStream() {
            logger_.log(level_, stream_.str(), file_, line_);
        }

        template<typename T>
        LogStream &operator<<(const T &value) {
            stream_ << value;
            return *this;
        }

    private:
        Logger &logger_;
        Level level_;
        const char *file_;
        std::size_t line_;
        std::ostringstream stream_;
    };

public:
    Logger(const std::string &name, std::shared_ptr<Logger> parent)
        : name_(name), parent_(parent) {}
    /**
     * @brief Sets the threshold for this logger to level.
     *   Logging messages which are less severe than level will be ignored;
     *   logging messages which have severity level or higher will be emitted
     *   by whichever handler or handlers service this logger
     */
    void set_level(Level level) noexcept {
        level_ = level;
    }

    template<typename Handler>
    HandlerConnection add_handler(Handler &&handler) {
        std::lock_guard<std::mutex> lock(handlers_mutex_);
        signals::Connection conn_raw = handlers_.signal_interface().connect(std::move(handler));
        return HandlerConnection{std::move(conn_raw), handlers_mutex_};
    }

    template<typename Handler>
    void add_handler(std::shared_ptr<Handler> handler) {
        if (!handler) return;
        handlers_.signal_interface().connect([handler](const LogRecord &record) {
            (*handler)(record);
        });
    }

    /**
     * @detail
     *   DEBUG: Detailed information, typically of interest only when diagnosing problems.
     */
    void debug(const std::string &message, const char *file, std::size_t line) {
        handle(LogRecord{name_, Level::Debug, message, file, line});
    }

    /**
     * @detail
     *   INFO: Confirmation that things are working as expected.
     */
    void info(const std::string &message, const char *file, std::size_t line) {
        handle(LogRecord{name_, Level::Info, message, file, line});
    }
    /**
     * @detail
     *   WARNING: An indication that something unexpected happened, or indicative of some problem in the near future (e.g. 'disk space low').
     *       The software is still working as expected.
     */
    void warn(const std::string &message, const char *file, std::size_t line) {
        handle(LogRecord{name_, Level::Warn, message, file, line});
    }
    /**
     * @detail
     *   ERROR: Due to a more serious problem, the software has not been able to perform some function.
     */
    void error(const std::string &message, const char *file, std::size_t line) {
        handle(LogRecord{name_, Level::Error, message, file, line});
    }
    /**
     * @detail
     *   FATAL: A serious error, indicating that the program itself may be unable to continue running.
     */
    void fatal(const std::string &message, const char *file, std::size_t line) {
        handle(LogRecord{name_, Level::Fatal, message, file, line});
    }
    /**
     * @brief Logs a message with level.
     */
    void log(Level level, const std::string &message, const char *file, std::size_t line) {
        handle(LogRecord{name_, level, message, file, line});
    }

    LogStream debug(const char *file, std::size_t line) {
        return {*this, Level::Debug, file, line};
    }

    LogStream info(const char *file, std::size_t line) {
        return {*this, Level::Info, file, line};
    }

    LogStream warn(const char *file, std::size_t line) {
        return {*this, Level::Warn, file, line};
    }

    LogStream error(const char *file, std::size_t line) {
        return {*this, Level::Error, file, line};
    }

    LogStream fatal(const char *file, std::size_t line) {
        return {*this, Level::Fatal, file, line};
    }

    LogStream log(Level level, const char *file, std::size_t line) {
        return {*this, level, file, line};
    }

private:
    const std::string name_;
    std::atomic<Level> level_{Level::Unset};
    std::shared_ptr<Logger> parent_;

    std::mutex handlers_mutex_;
    signals::Signal<void(const LogRecord &)> handlers_;

    NODEC_DISABLE_COPY(Logger)
};

} // namespace logging
} // namespace nodec
#endif