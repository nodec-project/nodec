#include "sub.hpp"

#include <iostream>

#include <nodec/logging.hpp>
#include <nodec/event.hpp>

#include <string>
#include <fstream>
#include <sstream>


class Logger 
{
public:
    Logger()
    {
        log_file.open("output.log", std::ios::binary);

    }

    void record_to_file_handler(const nodec::logging::LogRecord& record) noexcept
    {
        log_file << record << "\n";
    }

public:
    std::ofstream log_file;
    nodec::logging::MemberRecordHandler<Logger>::SharedPtr handler;
};

int main()
{
    auto logger = std::make_shared<Logger>();
    logger->handler = nodec::logging::MemberRecordHandler<Logger>::make_shared(logger, &Logger::record_to_file_handler);
    nodec::logging::record_handlers += logger->handler;

    std::cout << "--- 1 ---" << std::endl;

    std::string message = "test message";

    //nodec::logging::formatter(message);
    std::cout << message << std::endl;
    
    nodec::logging::Level level;
    level = nodec::logging::Level::Debug;

    std::cout << level << std::endl;


    std::cout << "--- 2 ---" << std::endl;
    nodec::logging::LogRecord record(nodec::logging::Level::Error, "are you ok?", __FILE__, __LINE__);
    std::cout << record.message << std::endl;
    nodec::logging::LogRecord record_copy = record;
    std::cout << record_copy.message << std::endl;

    std::cout << nodec::logging::formatter(record) << std::endl;


    std::cout << "--- 3 ---" << std::endl;
    {
        auto record_handler_ptr = nodec::logging::StaticRecordHandler::make_shared(nodec::logging::record_to_stdout_handler);
        nodec::logging::record_handlers += record_handler_ptr;
    }
    nodec::logging::debug("debug message.", __FILE__, __LINE__);
    nodec::logging::info("info message.", __FILE__, __LINE__);
    nodec::logging::warn("warn message.", __FILE__, __LINE__);
    nodec::logging::error("error message.", __FILE__, __LINE__);
    nodec::logging::fatal("fatal message.", __FILE__, __LINE__);


    std::cout << "--- 4 ---" << std::endl;
    
    nodec::logging::set_level(nodec::logging::Level::Error);
    nodec::logging::debug("debug message.", __FILE__, __LINE__);
    nodec::logging::info("info message.", __FILE__, __LINE__);
    nodec::logging::warn("warn message.", __FILE__, __LINE__);
    nodec::logging::error("error message.", __FILE__, __LINE__);
    nodec::logging::fatal("fatal message.", __FILE__, __LINE__);


    std::cout << "--- 5 ---" << std::endl;
    nodec::logging::set_level(nodec::logging::Level::Info);
    sub_func();
    //nodec::logging::formatter = ;

    std::cout << "--- 6 ---" << std::endl;
    nodec::logging::DebugStream(__FILE__, __LINE__) << "Debug" << std::endl;
    nodec::logging::FatalStream(__FILE__, __LINE__) << "Fatal\n"
        << "OKOK\n" 
        << "No problem!" << std::flush;;

    std::cout << "--- 7 ---" << std::endl;
    {
        nodec::logging::ErrorStream error_stream(__FILE__, __LINE__);
        error_stream << "ERROR";
        error_stream << nodec::logging::Level::Debug;
    }
    nodec::logging::InfoStream(__FILE__, __LINE__) << "info" << std::flush << std::flush;

    //std::ostringstream() << "A" << nodec::logging::Level::Debug;

    //oss << "TEST";
    //oss << nodec::logging::Level::Debug << "A";
    //nodec::logging::InfoStream(__FILE__, __LINE__) << nodec::logging::Level::Debug;
}