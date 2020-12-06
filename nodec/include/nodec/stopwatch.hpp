#ifndef NODEC__STOPWATCH_HPP_
#define NODEC__STOPWATCH_HPP_


#include <chrono>

namespace nodec
{
    template<typename ClockT>
    class Stopwatch
    {
    public:
        Stopwatch() :
            is_running_(false)
        {
        }

        ~Stopwatch() {}

    public:

        bool is_running() const noexcept { return is_running_; }

        void start()
        {
            if (is_running_)
            {
                return;
            }
            start_time = ClockT::now();
            checkpoint_time = ClockT::now();
            is_running_ = true;
        }

        void stop()
        {
            //std::chrono::system_clock::
            if (!is_running_)
            {
                return;
            }
            stop_time = ClockT::now();
            is_running_ = false;
        }

        void reset()
        {
            stop();
            start_time = ClockT::time_point();
            stop_time = ClockT::time_point();
            checkpoint_time = ClockT::time_point();
        }

        void restart()
        {
            reset();
            start();
        }

        typename ClockT::duration lap()
        {
            auto current_time_ = current_time();
            auto lap_time = current_time_ - checkpoint_time;
            checkpoint_time = current_time_;
            return lap_time;
        }

        typename ClockT::duration elapsed() const 
        {
            return current_time() - start_time;
        }

    private:
        std::chrono::time_point<ClockT> current_time() const 
        {
            std::chrono::steady_clock::now();
            auto current_time_ = stop_time;
            if (is_running_)
            {
                current_time_ = ClockT::now();
            }
            return current_time_;
        }

    private:
        std::chrono::time_point<ClockT> start_time;
        std::chrono::time_point<ClockT> stop_time;
        std::chrono::time_point<ClockT> checkpoint_time;
        bool is_running_;

    };
}

#endif