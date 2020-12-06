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

        LogRecord::LogRecord(Level level, const std::string& message, const char* file, size_t line) :
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
            oss << "[" << record.file << " line " << record.line << "]\n";
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

            oss << record.message << "\n";

            return oss.str();
        }

        void record_to_stdout_handler(const LogRecord& record) noexcept
        {
            std::cout << record << std::endl;
        }

        std::function <std::string(const LogRecord&)> formatter = default_formatter;
        nodec::event::Event<const LogRecord&> record_handlers;

        static void log_generic(const LogRecord& record)
        {
            if (record.level < current_level_)
            {
                // ignore this log
                return;
            }

            // while Handler objects are dispatching the appropriate log messages (based on the log messages' severity) 
            // to the handler's specified destination, lock the event to invoke.
            io_lock_.lock();
            record_handlers.invoke(record);
            io_lock_.unlock();
        }


        void debug(const std::string& message, const char* file, size_t line)
        {
            log_generic(LogRecord{ Level::Debug, message, file, line });
        }

        void info(const std::string& message, const char* file, size_t line)
        {
            log_generic(LogRecord{ Level::Info, message, file, line });
        }

        void warn(const std::string& message, const char* file, size_t line)
        {
            log_generic(LogRecord{ Level::Warn, message, file, line });
        }

        void error(const std::string& message, const char* file, size_t line)
        {
            log_generic(LogRecord{ Level::Error, message, file, line });
        }

        void fatal(const std::string& message, const char* file, size_t line)
        {
            log_generic(LogRecord{ Level::Fatal, message, file, line });
        }

        void log(Level level, const std::string& message, const char* file, size_t line)
        {
            log_generic(LogRecord{ level, message, file, line });
        }

        class LogStreamBufferGeneric : public std::streambuf
        {
            using Base = std::streambuf;
        public:
            LogStreamBufferGeneric(const Level level) :
                level(level),
                file(nullptr),
                line(0)
            {

            }
        public:
            void set(const char* _file, size_t _line)
            {
                file = _file;
                line = _line;
            }

        public:
            int overflow(int ch) override
            {
                message += ch;
                return ch;
            }
            int sync() override
            {
                log_generic(LogRecord{ level, message, file, line });
                message.clear();
                return 0; // not -1: means it ok.
            }
        private:
            std::string message;
            Level level;
            const char* file;
            size_t line;
        };

        static LogStreamBufferGeneric debug_stream_buffer_(Level::Debug);
        static LogStreamBufferGeneric info_stream_buffer_(Level::Info);
        static LogStreamBufferGeneric warn_stream_buffer_(Level::Warn);
        static LogStreamBufferGeneric error_stream_buffer_(Level::Error);
        static LogStreamBufferGeneric fatal_stream_buffer_(Level::Fatal);

        static std::ostream debug_stream_(&debug_stream_buffer_);
        static std::ostream info_stream_(&info_stream_buffer_);
        static std::ostream warn_stream_(&warn_stream_buffer_);
        static std::ostream error_stream_(&error_stream_buffer_);
        static std::ostream fatal_stream_(&fatal_stream_buffer_);

        
        std::ostream& debug_stream(const char* file, size_t line)
        {
            debug_stream_buffer_.set(file, line);
            return debug_stream_;
        }
        std::ostream& info_stream(const char* file, size_t line)
        {
            info_stream_buffer_.set(file, line);
            return info_stream_;
        }
        std::ostream& warn_stream(const char* file, size_t line)
        {
            warn_stream_buffer_.set(file, line);
            return warn_stream_;
        }
        std::ostream& error_stream(const char* file, size_t line)
        {
            error_stream_buffer_.set(file, line);
            return error_stream_;
        }
        std::ostream& fatal_stream(const char* file, size_t line)
        {
            fatal_stream_buffer_.set(file, line);
            return fatal_stream_;
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