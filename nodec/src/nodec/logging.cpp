#include <nodec/logging.hpp>

#include <iostream>
#include <mutex>
#include <sstream>

namespace nodec
{
    namespace logging
    {
        static std::mutex io_lock_;
        static Level current_level_ = Level::Debug;

        LogRecord::LogRecord(Level level, std::string message, const char* file, size_t line) :
            level(level),
            message(message),
            file(file),
            line(line)
        {
        }

        void set_level(Level level) { current_level_ = level; }

        std::string default_formatter(const LogRecord& record) noexcept
        {
            std::ostringstream oss;
            switch (record.level)
            {
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

            oss << record.message << std::endl
                << "[File] " << record.file << std::endl
                << "[Line] " << record.line << std::endl;

            return oss.str();
        }

        void record_to_stdout_handler(const LogRecord& record) noexcept
        {
            std::cout << record << std::endl;
        }

        std::function <std::string(const LogRecord&)> formatter = default_formatter;
        nodec::event::Event<const LogRecord&> record_handlers;

        static void log_generic_(const LogRecord& record)
        {
            if (record.level < current_level_)
            {
                // ignore this log
                return;
            }

            // while Handler objects are dispatching the appropriate log messages (based on the log messagesf severity) 
            // to the handlerfs specified destination, lock the event to invoke.
            io_lock_.lock();
            record_handlers.invoke(record);
            io_lock_.unlock();
        }



        void debug(std::string message, const char* file, size_t line)
        {
            log_generic_(LogRecord{ Level::Debug, message, file, line });
        }

        void info(std::string message, const char* file, size_t line)
        {
            log_generic_(LogRecord{ Level::Info, message, file, line });
        }

        void warn(std::string message, const char* file, size_t line)
        {
            log_generic_(LogRecord{ Level::Warn, message, file, line });
        }

        void error(std::string message, const char* file, size_t line)
        {
            log_generic_(LogRecord{ Level::Error, message, file, line });
        }

        void fatal(std::string message, const char* file, size_t line)
        {
            log_generic_(LogRecord{ Level::Fatal, message, file, line });
        }

    }
}
std::ostream& operator<<(std::ostream& stream, const nodec::logging::Level& level)
{
    switch (level)
    {
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

std::ostream& operator<<(std::ostream& stream, const nodec::logging::LogRecord& record)
{
    return stream << nodec::logging::formatter(record);
}