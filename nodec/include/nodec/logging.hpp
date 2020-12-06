#ifndef NODEC__LOGGING_HPP_
#define NODEC__LOGGING_HPP_

#include <nodec/event.hpp>

#include <functional>
#include <string>
#include <ostream>

namespace nodec
{
    namespace logging
    {
        enum class Level
        {
            Unset = 0,  //! The unset log level
            Debug = 10, //! The debug log level
            Info = 20,  //! The info log level
            Warn = 30,  //! The warn log level
            Error = 40, //! The error log level
            Fatal = 50  //! The fatal log level
        };

        struct LogRecord
        {
            LogRecord(Level level, const std::string& message, const char* file, size_t line);
            Level level;
            std::string message;
            const char* file;
            size_t line;
        };

        std::string default_formatter(const LogRecord& record) noexcept;
        void record_to_stdout_handler(const LogRecord& record) noexcept;

        extern std::function <std::string(const LogRecord&)> formatter;
        extern nodec::event::Event<const LogRecord&> record_handlers;

        using StaticRecordHandler = nodec::event::StaticCallback<const LogRecord&>;

        template<typename T>
        using MemberRecordHandler = typename nodec::event::MemberCallback<T, const LogRecord&>;


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
        void debug(const std::string& message, const char* file, size_t line);

        /**
        * @detail
        *   INFO: Confirmation that things are working as expected.
        */
        void info(const std::string& message, const char* file, size_t line);

        /**
        * @detail
        *   WARNING: An indication that something unexpected happened, or indicative of some problem in the near future (e.g. 'disk space low'). 
        *       The software is still working as expected.
        */
        void warn(const std::string& message, const char* file, size_t line);

        /**
        * @detail
        *   ERROR: Due to a more serious problem, the software has not been able to perform some function.
        */
        void error(const std::string& message, const char* file, size_t line);

        /**
        * @detail
        *   FATAL: A serious error, indicating that the program itself may be unable to continue running.
        */
        void fatal(const std::string& message, const char* file, size_t line);

        /**
        * @brief Logs a message with level.
        */
        void log(Level level, const std::string& message, const char* file, size_t line);


        std::ostream& debug_stream(const char* file, size_t line);
        std::ostream& info_stream(const char* file, size_t line);
        std::ostream& warn_stream(const char* file, size_t line);
        std::ostream& error_stream(const char* file, size_t line);
        std::ostream& fatal_stream(const char* file, size_t line);
    }
}

std::ostream& operator<<(std::ostream& stream, const nodec::logging::Level& level);

std::ostream& operator<<(std::ostream& stream, const nodec::logging::LogRecord& record);

#endif