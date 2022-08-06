#ifndef NODEC__STOPWATCH_HPP_
#define NODEC__STOPWATCH_HPP_

// This code is based on...
//  * https://github.com/astagi/lauda
//
// Thank you :)

#include <chrono>

namespace nodec {

template<typename ClockT>
class Stopwatch {
public:
    // using snake_case like std.
    using clock = ClockT;
    using duration = typename clock::duration;
    using time_point = typename clock::time_point;

public:
    Stopwatch()
        : is_running_(false) {
    }

    ~Stopwatch() {}

public:
    bool is_running() const noexcept {
        return is_running_;
    }

    void start() {
        if (is_running_) return;

        start_time = clock::now();
        checkpoint_time = clock::now();
        is_running_ = true;
    }

    void stop() {
        if (!is_running_) return;

        stop_time = clock::now();
        is_running_ = false;
    }

    void reset() {
        stop();
        start_time = time_point();
        stop_time = time_point();
        checkpoint_time = time_point();
    }

    void restart() {
        reset();
        start();
    }

    duration lap() {
        const auto current_time_ = (is_running_ ? clock::now() : stop_time);
        const auto lap_time = current_time_ - checkpoint_time;
        checkpoint_time = current_time_;
        return lap_time;
    }

    duration elapsed() const {
        return ((is_running_ ? clock::now() : stop_time)) - start_time;
    }

private:
    time_point start_time;
    time_point stop_time;
    time_point checkpoint_time;
    bool is_running_;
};

} // namespace nodec

#endif