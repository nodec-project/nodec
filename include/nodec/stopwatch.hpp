#ifndef NODEC__STOPWATCH_HPP_
#define NODEC__STOPWATCH_HPP_

// This code is based on...
//  * https://github.com/astagi/lauda
//  * https://github.com/ravener/stopwatch.py
//
// Thank you :)

#include <chrono>
#include <ratio>

namespace nodec {

template<typename ClockT>
class BasicStopwatch {
public:
    // using snake_case like std.
    using clock = ClockT;
    using duration = typename clock::duration;
    using time_point = typename clock::time_point;

public:
    BasicStopwatch() {}

    ~BasicStopwatch() {}

public:
    bool is_running() const noexcept {
        return !has_end_time_;
    }

    void start() {
        if (!has_end_time_) return;

        start_time_ = clock::now() - elapsed();
        checkpoint_time_ = start_time_;
        has_end_time_ = false;
    }

    void stop() {
        if (has_end_time_) return;

        end_time_ = clock::now();
        has_end_time_ = true;
    }

    void reset() {
        start_time_ = clock::now();
        end_time_ = start_time_;
        checkpoint_time_ = start_time_;
        has_end_time_ = true;
    }

    void restart() {
        start_time_ = clock::now();
        checkpoint_time_ = start_time_;
        has_end_time_ = false;
    }

    duration lap() {
        const auto current_time = has_end_time_ ? end_time_ : clock::now();
        const auto lap_time = current_time - checkpoint_time_;

        checkpoint_time_ = current_time;
        return lap_time;
    }

    template<class Rep, class Period = std::ratio<1>>
    std::chrono::duration<Rep, Period> lap() {
        return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(lap());
    }

    duration elapsed() const {
        return has_end_time_ ? (end_time_ - start_time_)
                             : (clock::now() - start_time_);
    }

    template<class Rep, class Period = std::ratio<1>>
    std::chrono::duration<Rep, Period> elapsed() const {
        return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(elapsed());
    }

private:
    time_point start_time_;
    time_point end_time_;
    time_point checkpoint_time_;
    bool has_end_time_{true};
};

using Stopwatch = BasicStopwatch<std::chrono::steady_clock>;

} // namespace nodec

#endif