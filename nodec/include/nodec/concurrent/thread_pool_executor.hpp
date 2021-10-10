#ifndef NODEC__CONCURRENT__THREAD_POOL_EXECUTOR_HPP_
#define NODEC__CONCURRENT__THREAD_POOL_EXECUTOR_HPP_

#include <nodec/logging.hpp>

#include <thread>
#include <cstdint>
#include <atomic>
#include <mutex>
#include <queue>
#include <functional>
#include <memory>
#include <future>
#include <type_traits>


namespace nodec {
namespace concurrent {

// This code based on 
//  * <https://github.com/bshoshany/thread-pool>
//  * <https://github.com/progschj/ThreadPool>
//  * <https://github.com/SandSnip3r/thread-pool>
// Thank you! :)


class ThreadPoolExecutor {

    using ui32 = std::uint_fast32_t;
    using ui64 = std::uint_fast64_t;

public:

    ThreadPoolExecutor(const ui32& thread_count = std::thread::hardware_concurrency())
        : thread_count_{ thread_count ? thread_count : std::thread::hardware_concurrency() } {
        threads.reset(new std::thread[thread_count_]);

        for (ui32 i = 0; i < thread_count_; ++i) {
            threads[i] = std::thread(&ThreadPoolExecutor::worker, this);
        }
    }

    /**
    * @brief Destruct the thread pool. Waits for all tasks to complete, then destroys all thread.
    *
    */
    ~ThreadPoolExecutor() {
        {
            // Lock task queue to prevent adding new task.
            std::lock_guard<std::mutex> lock(tasks_mutex);
            running = false;
        }

        // Wake up all threads so that they may exist
        condition.notify_all();

        for (ui32 i = 0; i < thread_count_; ++i) {
            threads[i].join();
        }
    }

    ui32 thread_count() const {
        return thread_count_;
    }

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    /**
    * @brief Submit a function with zero or more arguments and a return value into the task queue,
    * and get a future for its eventual returned value.
    */
    template <typename F, typename... Args, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
#else
    template <typename F, typename... Args, typename R = typename std::result_of<std::decay_t<F>(std::decay_t<Args>...)>::type>
#endif
    std::future<R> submit(F&& func, const Args&&... args) {
        auto task = std::make_shared<std::packaged_task<R()>>([func, args...]() {
            return func(args...);
        });
        auto future = task->get_future();

        push_task([task]() { (*task)(); });
        return future;
    }

private:

    template <typename F>
    void push_task(const F& task) {
        {
            const std::lock_guard<std::mutex> lock(tasks_mutex);

            if (!running) {
                throw std::runtime_error("Cannot schedule new task after shutdown.");
            }

            tasks.push(std::function<void()>(task));
        }

        condition.notify_one();
    }


    /**
    * @brief A worker function to be assigned to each thread in the pool.
    *
    *  Continuously pops tasks out of the queue and executes them, as long as the atomic variable running is set to true.
    */
    void worker() {
        for (;;) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(tasks_mutex);
                condition.wait(lock, [&] {return !tasks.empty() || !running; });

                if (!running && tasks.empty()) {
                    return;
                }

                task = std::move(tasks.front());
                tasks.pop();
            }

            task();
        }
    }

private:

    /**
    * @brief A mutex to synchronize access to the task queue by different threads.
    */
    mutable std::mutex tasks_mutex{};

    /**
    * @brief An atomic variable indicating to the workers to keep running.
    *
    * When set to false, the workers permanently stop working.
    */
    std::atomic<bool> running{ true };

    /**
    * @brief A queue of tasks to be executed by the threads.
    */
    std::queue<std::function<void()>> tasks{};

    /**
    * @brief The number of threads in the pool.
    */
    const ui32 thread_count_;

    /**
    * @brief A smart pointer to manage the memory allocated for the threads.
    */
    std::unique_ptr<std::thread[]> threads;

    /**
     * @brief A condition variable used to notify worker threads of state changes.
     */
    std::condition_variable condition;
};

}
}

#endif