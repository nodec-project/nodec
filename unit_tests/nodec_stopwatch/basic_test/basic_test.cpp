#include <nodec/stopwatch.hpp>
#include <nodec/logging.hpp>

#include <iostream>

#include <chrono>
int main()
{
    auto logging_callback = nodec::event::StaticCallback<const nodec::logging::LogRecord&>::make_shared(nodec::logging::record_to_stdout_handler);
    nodec::logging::record_handlers += logging_callback;

    nodec::logging::info("logging start", __FILE__, __LINE__);

    
    nodec::logging::debug(typeid(std::chrono::steady_clock::now().time_since_epoch()).name(),
        __FILE__, __LINE__);

    std::cout << std::chrono::steady_clock::now().time_since_epoch().count() << std::endl;

    nodec::Stopwatch<std::chrono::steady_clock> sw;
    //nodec::Stopwatch<float> sw;
    sw.current_time();
}