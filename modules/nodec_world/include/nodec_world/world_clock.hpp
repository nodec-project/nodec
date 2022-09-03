#ifndef NODEC_WORLD__WORLD_CLOCK_HPP_
#define NODEC_WORLD__WORLD_CLOCK_HPP_

#include <nodec/stopwatch.hpp>

namespace nodec_world {

class WorldClock {
    using Stopwatch = nodec::Stopwatch<std::chrono::steady_clock>;

public:
    using duration = std::chrono::duration<float>;

    class WorldClockInterface {
    public:
        WorldClockInterface(WorldClock *clock)
            : clock_{clock} {}

        float current_time() const noexcept {
            return clock_->current_time();
        }

        void set_time_scale(float scale) noexcept {
            return clock_->set_time_scale(scale);
        }

        float time_scale() const noexcept {
            return clock_->time_scale();
        }

        float delta_time() const noexcept {
            return clock_->delta_time();
        }

    private:
        WorldClock *clock_;
    };

    WorldClockInterface clock_interface() noexcept {
        return {this};
    }

    float current_time() const noexcept {
        return current_time_.count();
    }

    float delta_time() const noexcept {
        return delta_time_.count();
    }

    void set_time_scale(float scale) noexcept {
        time_scale_ = scale;
    }

    float time_scale() const noexcept {
        return time_scale_;
    }

    void reset() noexcept {
        current_time_ = duration{0};
        delta_time_ = duration{0};
        world_stopwatch_.reset();
    }

    void tick(float delta_sec) noexcept {
        delta_time_ = duration{delta_sec * time_scale_};
        current_time_ += delta_time_;
        world_stopwatch_.reset();
    }

    void tick() {
        using namespace std::chrono;
        auto delta = world_stopwatch_.elapsed();
        delta_time_ = delta * time_scale_;
        current_time_ += delta_time_;
        world_stopwatch_.restart();
    }

private:
    Stopwatch world_stopwatch_;
    float time_scale_{1.0f};

    duration current_time_;
    duration delta_time_;
};
} // namespace nodec_world

#endif