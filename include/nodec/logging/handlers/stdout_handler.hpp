#ifndef NODEC__LOGGING__HANDLERS__STDOUT_HANDLER_HPP_
#define NODEC__LOGGING__HANDLERS__STDOUT_HANDLER_HPP_

#include <iostream>
#include <mutex>

#include "../log_record.hpp"

namespace nodec {
namespace logging {
namespace handlers {

struct ConsoleMutex {
    static std::mutex &get() {
        static std::mutex mutex;
        return mutex;
    }
};

template<class Formatter>
class StdoutHandler {
private:
public:
    StdoutHandler(Formatter formatter = Formatter{})
        : formatter_(formatter) {}

    void operator()(const LogRecord &record) {
        std::lock_guard<std::mutex> lock(ConsoleMutex::get());
        std::cout << formatter_(record) << std::flush;
    }

private:
    Formatter formatter_;
};
} // namespace handlers
} // namespace logging
} // namespace nodec
#endif