#ifndef NODEC__EVENT_LOOP_HPP_
#define NODEC__EVENT_LOOP_HPP_

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>
#include <vector>

namespace nodec::asyncio {

/**
 * @brief Handles asynchronous event processing.
 *
 * EventLoop allows scheduling and processing of events, similar to Python's asyncio.
 * It does not create its own thread - the caller must call spin() or spin_once()
 * to process events. Events can be scheduled with delays.
 */
class EventLoop {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;
    using EventCallback = std::function<void()>;

    /**
     * @brief Event task with scheduled execution time.
     */
    struct Event {
        TimePoint scheduled_time;
        EventCallback callback;

        // For priority queue ordering (earliest time first)
        bool operator>(const Event &other) const {
            return scheduled_time > other.scheduled_time;
        }
    };

    EventLoop() = default;
    ~EventLoop() = default;

    /**
     * @brief Thread-safe version of schedule_after.
     *
     * This method can be safely called from any thread.
     *
     * @param callback Function to be called when event is processed
     * @param delay Time to wait before executing the event
     */
    template<typename Callable>
    void schedule(Callable &&callback, Duration delay = Duration::zero()) {
        Event event{
            Clock::now() + delay,
            std::forward<Callable>(callback)};

        {
            std::lock_guard<std::mutex> lock(mutex_);
            event_queue_.push(std::move(event));
        }
        cv_.notify_one(); // Notify waiting threads
    }

    /**
     * @brief Process all pending events.
     *
     * Processes events that are due at the time of calling.
     * This implementation minimizes memory allocation and copying.
     *
     * @param max_events Maximum number of events to process in a single call (0 = unlimited)
     * @return Number of events processed
     */
    size_t spin_once(size_t max_events = 0) {
        const auto now = Clock::now();
        size_t processed = 0;

        // Process events one by one to minimize the lock duration
        while (processed < max_events || max_events == 0) {
            EventCallback callback;
            bool has_event = false;

            // Extract a single event under the lock
            {
                std::lock_guard<std::mutex> lock(mutex_);

                if (event_queue_.empty() || event_queue_.top().scheduled_time > now) {
                    // No more events ready to process
                    break;
                }

                // Move the callback out of the event
                callback = std::move(event_queue_.top().callback);
                event_queue_.pop();
                has_event = true;
            }

            // Execute the callback outside the lock
            if (has_event) {
                callback();
                processed++;
            }
        }

        return processed;
    }

    /**
     * @brief Check if there are pending events.
     *
     * @return true if there are events in the queue
     */
    bool has_pending_events() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !event_queue_.empty();
    }

    /**
     * @brief Wait for events without busy looping.
     *
     * This function uses a condition variable to efficiently wait for events.
     * It will wait until an event is ready to be processed or until a new event is scheduled.
     *
     * @param max_wait Maximum time to wait. Use Duration::max() to wait indefinitely.
     * @return true if there are events ready to process after waiting, false otherwise
     */
    bool wait_for_events(Duration max_wait = Duration::max()) {
        std::unique_lock<std::mutex> lock(mutex_);

        if (event_queue_.empty()) {
            return false; // No events to process
        }

        // We have events, but they might not be ready yet
        TimePoint next_event_time = event_queue_.top().scheduled_time;
        auto now = Clock::now();

        if (next_event_time <= now) {
            // Event is already ready to process
            return true;
        }

        // Calculate time to wait until the next event is ready
        Duration wait_time = next_event_time - now;

        // Don't wait longer than max_wait
        if (max_wait < wait_time) {
            wait_time = max_wait;
        }

        // Wait until:
        // 1. A new event is scheduled (possibly with an earlier execution time), or
        // 2. The wait time expires (next event is ready)
        bool new_event_scheduled = cv_.wait_for(lock, wait_time, [this, next_event_time] {
            return event_queue_.empty() || event_queue_.top().scheduled_time < next_event_time;
        });

        // Either the next event is ready to process, or a new event was scheduled
        return !event_queue_.empty() && (Clock::now() >= event_queue_.top().scheduled_time || new_event_scheduled);
    }

    /**
     * @brief Process events with efficient waiting.
     *
     * This function combines waiting and processing events.
     * It waits until events are ready to process, then processes them.
     * Unlike spin(), this function avoids busy loops by using condition variables.
     *
     * @param max_wait Maximum time to wait for an event. Use Duration::max() to wait indefinitely.
     * @return Number of events processed
     */
    void spin(Duration max_wait = Duration::max()) {
        while (wait_for_events(max_wait)) {
            spin_once();
        }
        return;
    }

private:
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> event_queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_; // Condition variable for waiting on events
};

} // namespace nodec::asyncio

#endif // NODEC__EVENT_LOOP_HPP_