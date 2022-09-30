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

    nodec::logging::debug_stream(__FILE__, __LINE__) << std::chrono::steady_clock::now().time_since_epoch().count() << std::flush;

    nodec::Stopwatch<std::chrono::steady_clock> sw;
    sw.restart();
    for (auto i = 0; i < 10000000; i++);
    nodec::logging::info_stream(__FILE__, __LINE__) << std::chrono::duration<float>(sw.elapsed()).count() << std::flush;
    for (auto i = 0; i < 10000000; i++);
    nodec::logging::info_stream(__FILE__, __LINE__) << std::chrono::duration<float>(sw.elapsed()).count() << std::flush;
    for (auto i = 0; i < 10000000; i++);
    nodec::logging::info_stream(__FILE__, __LINE__) << std::chrono::duration<float>(sw.elapsed()).count() << std::flush;

    sw.stop();
    sw.reset();
    nodec::logging::info_stream(__FILE__, __LINE__) << std::chrono::duration<float>(sw.elapsed()).count() << std::flush;
    sw.start();

    for (auto i = 0; i < 10000000; i++);
    nodec::logging::info_stream(__FILE__, __LINE__) << std::chrono::duration<float>(sw.lap()).count() << std::flush;
    for (auto i = 0; i < 10000000; i++);
    nodec::logging::info_stream(__FILE__, __LINE__) << std::chrono::duration<float>(sw.lap()).count() << std::flush;
    for (auto i = 0; i < 10000000; i++);
    nodec::logging::info_stream(__FILE__, __LINE__) << std::chrono::duration<float>(sw.lap()).count() << std::flush;

    //nodec::Stopwatch<float> sw;


}