#ifndef NODEC__STOPWATCH_HPP_
#define NODEC__STOPWATCH_HPP_

#include <chrono>

namespace nodec
{
    template<typename ClockT>
    class Stopwatch
    {
    public:
        Stopwatch() {}
        ~Stopwatch() {}

    public:
        void start();
        void stop();
        void reset();
        void restart();
        float lap();
        bool is_running();

    public:
        float current_time()
        {
            is_running_ = true;
            auto current_time_ = stop_time;
            if (is_running_)
            {
                std::chrono::nanoseconds nanos = ClockT::now().time_since_epoch();
                
            }
            return 0.0f;
        }

    private:
        float start_time;
        float stop_time;
        float checkpoint_time;
        bool is_running_;

    };
}

#endif