#ifndef NODEC__EVENT_PROMISE_HPP_
#define NODEC__EVENT_PROMISE_HPP_

#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>

#include "event_loop.hpp"

namespace nodec::asyncio {

template<typename T>
class EventPromise;

// Type trait to detect EventPromise specializations
template<typename T>
struct is_event_promise : std::false_type {};

template<typename T>
struct is_event_promise<EventPromise<T>> : std::true_type {};

// Helper for safely extracting value_type from EventPromise or falling back to the type itself
template<typename T, typename = void>
struct value_type_or_self {
    using type = T;
};

// Specialization for EventPromise types
template<typename T>
struct value_type_or_self<T, std::enable_if_t<is_event_promise<T>::value>> {
    using type = typename std::remove_reference_t<T>::value_type;
};

enum class PromiseState {
    /**
     * @brief Initial state: Promise not yet fulfilled or rejected. Operation is in progress.
     *
     * Promise starts in this state and can transition to either Fulfilled or Rejected.
     */
    Pending,

    /**
     * @brief Success state: Promise operation completed successfully with a value.
     *
     * Reached when resolve() is called. Triggers callbacks registered with then().
     * Once in this state, the promise remains fulfilled (immutable).
     */
    Fulfilled,

    /**
     * @brief Error state: Promise operation failed with an exception.
     *
     * Reached when reject() is called or when an exception occurs during processing.
     * Contains error information as std::exception_ptr and triggers catch_error() handlers.
     * Once in this state, the promise remains rejected (immutable).
     */
    Rejected
};

namespace detail {

// Shared class for holding the internal state of promises
template<typename T>
struct PromiseData {
    PromiseState state = PromiseState::Pending;
    EventLoop &event_loop;
    std::variant<std::monostate, T, std::exception_ptr> value;

    std::function<void(const T &)> on_fulfill;
    std::function<void(std::exception_ptr)> on_reject;

    PromiseData(EventLoop &el): event_loop(el) {}
};

// Helper structure for void specialization
template<typename T>
struct PromiseDataVoid {
    // Standard implementation for non-void types
    using type = PromiseData<T>;
};

// Void specialization
template<>
struct PromiseDataVoid<void> {
    // Specialized type for void
    struct type {
        PromiseState state = PromiseState::Pending;
        EventLoop &event_loop;
        std::variant<std::monostate, std::exception_ptr> value;

        std::function<void()> on_fulfill;
        std::function<void(std::exception_ptr)> on_reject;

        type(EventLoop &el): event_loop(el) {}
    };
};

// Helper alias template
template<typename T>
using PromiseDataType = typename PromiseDataVoid<T>::type;

// Helper for value operations
template<typename T>
struct ValueHelper {
    static void store_value(std::variant<std::monostate, T, std::exception_ptr> &var, const T &val) {
        var = val;
    }

    static void store_value(std::variant<std::monostate, T, std::exception_ptr> &var, T &&val) {
        var = std::forward<T>(val);
    }

    template<typename Func>
    static auto invoke_with_value(Func &&func, const T &val)
        -> decltype(func(val)) {
        return func(val);
    }
};

// Void specialization
template<>
struct ValueHelper<void> {
    static void store_value(std::variant<std::monostate, std::exception_ptr> & /*var*/) {
        // Nothing to do for void type
    }

    template<typename Func>
    static auto invoke_with_value(Func &&func)
        -> decltype(func()) {
        return func();
    }
};
} // namespace detail

/**
 * @brief JavaScript-like Promise class for chainable asynchronous processing
 *
 * @tparam T Type of value this promise resolves to (can be void)
 */
template<typename T>
class EventPromise {
public:
    using value_type = T;
    using data_type = detail::PromiseDataType<T>;
    using shared_data = std::shared_ptr<data_type>;

    // Function type used to create Promise
    using resolver_func = std::function<void(std::function<void(T)>, std::function<void(std::exception_ptr)>)>;

    /**
     * @brief Create a Promise with an EventLoop
     * @param event_loop EventLoop to use
     */
    EventPromise(EventLoop &event_loop)
        : data_(std::make_shared<data_type>(event_loop)) {}

    /**
     * @brief Create a Promise using a function
     *
     * @param event_loop EventLoop to use
     * @param executor Function that calls resolve and reject
     */
    EventPromise(EventLoop &event_loop, resolver_func executor)
        : EventPromise(event_loop) {
        try {
            // Capture data_ instead of this pointer (safe because it's a shared_ptr)
            auto data = data_;
            executor(
                [data](T value) {
                    if (data->state != PromiseState::Pending) return;
                    data->state = PromiseState::Fulfilled;
                    detail::ValueHelper<T>::store_value(data->value, std::move(value));
                    if (data->on_fulfill) {
                        const T &stored_value = std::get<T>(data->value);
                        data->on_fulfill(stored_value);
                    }
                },
                [data](std::exception_ptr err) {
                    if (data->state != PromiseState::Pending) return;
                    data->state = PromiseState::Rejected;
                    data->value = err;
                    if (data->on_reject) {
                        data->on_reject(std::get<std::exception_ptr>(data->value));
                    }
                });
        } catch (...) {
            auto data = data_;
            if (data->state == PromiseState::Pending) {
                data->state = PromiseState::Rejected;
                data->value = std::current_exception();
                if (data->on_reject) {
                    data->on_reject(std::get<std::exception_ptr>(data->value));
                }
            }
        }
    }

    /**
     * @brief Create a Promise from an already resolved value
     *
     * @param event_loop EventLoop to use
     * @param value Resolved value
     * @return Resolved Promise
     */
    static EventPromise<T> resolve(EventLoop &event_loop, T value) {
        EventPromise<T> promise(event_loop);
        promise.resolve(std::move(value));
        return promise;
    }

    /**
     * @brief Create a Promise from an already rejected exception
     *
     * @param event_loop EventLoop to use
     * @param error Exception pointer
     * @return Rejected Promise
     */
    static EventPromise<T> reject(EventLoop &event_loop, std::exception_ptr error) {
        EventPromise<T> promise(event_loop);
        promise.reject(std::move(error));
        return promise;
    }

    /**
     * @brief Chain operations to be called after this Promise resolves
     *
     * @tparam Func Function type
     * @param on_fulfilled Function called when promise is resolved
     * @return New Promise
     */
    template<typename Func>
    auto then(Func &&on_fulfilled) {
        using raw_result_type = decltype(on_fulfilled(std::declval<T>()));

        // If callback returns a Promise, use its value_type; otherwise use the raw result type
        // Using value_type_or_self to safely extract the type
        using result_type = typename value_type_or_self<raw_result_type>::type;

        using next_type = typename std::conditional<
            std::is_void<result_type>::value,
            void,
            result_type>::type;

        EventPromise<next_type> next_promise(data_->event_loop);

        // Create copies of variables to capture (for C++17 compatibility)
        auto next_data = next_promise.data_;
        auto fulfilled_func = std::forward<Func>(on_fulfilled);

        auto setup_fulfillment = [next_data, fulfilled_func](const T &value) {
            try {
                if constexpr (std::is_void<raw_result_type>::value) {
                    // Void return type
                    detail::ValueHelper<T>::invoke_with_value(fulfilled_func, value);
                    next_data->state = PromiseState::Fulfilled;
                    if (next_data->on_fulfill) {
                        next_data->on_fulfill();
                    }
                } else if constexpr (is_event_promise<raw_result_type>::value) {
                    // Promise return type - unwrap it
                    auto inner_promise = detail::ValueHelper<T>::invoke_with_value(fulfilled_func, value);

                    // Connect the inner promise to the outer one
                    if (inner_promise.data_->state == PromiseState::Fulfilled) {
                        if constexpr (std::is_void<result_type>::value) {
                            // Inner promise resolves to void
                            next_data->state = PromiseState::Fulfilled;
                            if (next_data->on_fulfill) {
                                next_data->on_fulfill();
                            }
                        } else {
                            // Inner promise resolves to a value
                            next_data->state = PromiseState::Fulfilled;
                            detail::ValueHelper<next_type>::store_value(
                                next_data->value,
                                std::get<typename raw_result_type::value_type>(inner_promise.data_->value));
                            if (next_data->on_fulfill) {
                                next_data->on_fulfill(std::get<next_type>(next_data->value));
                            }
                        }
                    } else if (inner_promise.data_->state == PromiseState::Rejected) {
                        // Forward rejection
                        next_data->state = PromiseState::Rejected;
                        next_data->value = std::get<std::exception_ptr>(inner_promise.data_->value);
                        if (next_data->on_reject) {
                            next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                        }
                    } else {
                        // Set up callbacks on the inner promise
                        auto inner_data = inner_promise.data_;

                        if constexpr (std::is_void<result_type>::value) {
                            inner_data->on_fulfill = [next_data]() {
                                next_data->state = PromiseState::Fulfilled;
                                if (next_data->on_fulfill) {
                                    next_data->on_fulfill();
                                }
                            };
                        } else {
                            inner_data->on_fulfill = [next_data](const typename raw_result_type::value_type &inner_value) {
                                next_data->state = PromiseState::Fulfilled;
                                detail::ValueHelper<next_type>::store_value(next_data->value, inner_value);
                                if (next_data->on_fulfill) {
                                    next_data->on_fulfill(std::get<next_type>(next_data->value));
                                }
                            };
                        }

                        inner_data->on_reject = [next_data](std::exception_ptr error) {
                            next_data->state = PromiseState::Rejected;
                            next_data->value = error;
                            if (next_data->on_reject) {
                                next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                            }
                        };
                    }
                } else {
                    // Normal value return type
                    auto next_value = detail::ValueHelper<T>::invoke_with_value(fulfilled_func, value);
                    next_data->state = PromiseState::Fulfilled;
                    detail::ValueHelper<next_type>::store_value(next_data->value, std::move(next_value));
                    if (next_data->on_fulfill) {
                        next_data->on_fulfill(std::get<next_type>(next_data->value));
                    }
                }
            } catch (...) {
                next_data->state = PromiseState::Rejected;
                next_data->value = std::current_exception();
                if (next_data->on_reject) {
                    next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                }
            }
        };

        auto setup_rejection = [next_data](std::exception_ptr error) {
            next_data->state = PromiseState::Rejected;
            next_data->value = error;
            if (next_data->on_reject) {
                next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
            }
        };

        if (data_->state == PromiseState::Fulfilled) {
            // Modified for C++17: explicitly capture needed variables in lambda
            auto &loop = data_->event_loop;
            auto data_copy = data_;

            loop.schedule([data_copy, setup_fulfillment]() {
                setup_fulfillment(std::get<T>(data_copy->value));
            });
        } else if (data_->state == PromiseState::Rejected) {
            auto &loop = data_->event_loop;
            auto data_copy = data_;

            loop.schedule([data_copy, setup_rejection]() {
                setup_rejection(std::get<std::exception_ptr>(data_copy->value));
            });
        } else {
            // Eliminate dependency on this pointer
            auto data_copy = data_;
            data_->on_fulfill = [data_copy, setup_fulfillment](const T &value) {
                auto &loop = data_copy->event_loop;
                loop.schedule([setup_fulfillment, value]() {
                    setup_fulfillment(value);
                });
            };

            data_->on_reject = [data_copy, setup_rejection](std::exception_ptr error) {
                auto &loop = data_copy->event_loop;
                loop.schedule([setup_rejection, error]() {
                    setup_rejection(error);
                });
            };
        }

        return next_promise;
    }

    /**
     * @brief Error handling catch method
     *
     * @tparam Func Error handler function type
     * @param on_rejected Function called when error occurs
     * @return New Promise
     */
    template<typename Func>
    auto catch_error(Func &&on_rejected) {
        using raw_result_type = decltype(on_rejected(std::declval<std::exception_ptr>()));

        // If callback returns a Promise, use its value_type; otherwise use the raw result type
        using result_type = typename value_type_or_self<raw_result_type>::type;

        using next_type = typename std::conditional<
            std::is_void<result_type>::value,
            T, // 戻り値がvoidの場合は元のPromiseタイプを維持
            result_type>::type;

        EventPromise<next_type> next_promise(data_->event_loop);

        // Create copies of variables to capture (for C++17 compatibility)
        auto next_data = next_promise.data_;
        auto rejected_func = std::forward<Func>(on_rejected);

        auto setup_fulfillment = [next_data](const T &value) {
            next_data->state = PromiseState::Fulfilled;
            detail::ValueHelper<next_type>::store_value(next_data->value, value);
            if (next_data->on_fulfill) {
                next_data->on_fulfill(std::get<next_type>(next_data->value));
            }
        };

        auto setup_rejection = [next_data, rejected_func](std::exception_ptr error) {
            try {
                if constexpr (std::is_void<result_type>::value) {
                    // void型を返す場合
                    rejected_func(error);
                    next_data->state = PromiseState::Fulfilled;
                    if (next_data->on_fulfill) {
                        next_data->on_fulfill();
                    }
                } else if constexpr (is_event_promise<raw_result_type>::value) {
                    // Promise型を返す場合はフラット化
                    auto inner_promise = rejected_func(error);

                    // 内部Promiseの状態に応じた処理
                    if (inner_promise.data_->state == PromiseState::Fulfilled) {
                        if constexpr (std::is_void<typename raw_result_type::value_type>::value) {
                            // 内部Promiseがvoidを解決する場合
                            next_data->state = PromiseState::Fulfilled;
                            if (next_data->on_fulfill) {
                                next_data->on_fulfill();
                            }
                        } else {
                            // 内部Promiseが値を解決する場合
                            next_data->state = PromiseState::Fulfilled;
                            detail::ValueHelper<next_type>::store_value(
                                next_data->value,
                                std::get<typename raw_result_type::value_type>(inner_promise.data_->value));
                            if (next_data->on_fulfill) {
                                next_data->on_fulfill(std::get<next_type>(next_data->value));
                            }
                        }
                    } else if (inner_promise.data_->state == PromiseState::Rejected) {
                        // 内部Promiseの拒否を伝播
                        next_data->state = PromiseState::Rejected;
                        next_data->value = std::get<std::exception_ptr>(inner_promise.data_->value);
                        if (next_data->on_reject) {
                            next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                        }
                    } else {
                        // 内部Promiseがまだ解決されていない場合はコールバックを設定
                        auto inner_data = inner_promise.data_;

                        if constexpr (std::is_void<typename raw_result_type::value_type>::value) {
                            inner_data->on_fulfill = [next_data]() {
                                next_data->state = PromiseState::Fulfilled;
                                if (next_data->on_fulfill) {
                                    next_data->on_fulfill();
                                }
                            };
                        } else {
                            inner_data->on_fulfill = [next_data](const typename raw_result_type::value_type &inner_value) {
                                next_data->state = PromiseState::Fulfilled;
                                detail::ValueHelper<next_type>::store_value(next_data->value, inner_value);
                                if (next_data->on_fulfill) {
                                    next_data->on_fulfill(std::get<next_type>(next_data->value));
                                }
                            };
                        }

                        inner_data->on_reject = [next_data](std::exception_ptr inner_error) {
                            next_data->state = PromiseState::Rejected;
                            next_data->value = inner_error;
                            if (next_data->on_reject) {
                                next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                            }
                        };
                    }
                } else {
                    // 通常の値を返す場合
                    auto recovery_value = rejected_func(error);
                    next_data->state = PromiseState::Fulfilled;
                    detail::ValueHelper<next_type>::store_value(next_data->value, std::move(recovery_value));
                    if (next_data->on_fulfill) {
                        next_data->on_fulfill(std::get<next_type>(next_data->value));
                    }
                }
            } catch (...) {
                next_data->state = PromiseState::Rejected;
                next_data->value = std::current_exception();
                if (next_data->on_reject) {
                    next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                }
            }
        };

        if (data_->state == PromiseState::Fulfilled) {
            auto &loop = data_->event_loop;
            auto data_copy = data_;

            loop.schedule([data_copy, setup_fulfillment]() {
                setup_fulfillment(std::get<T>(data_copy->value));
            });
        } else if (data_->state == PromiseState::Rejected) {
            auto &loop = data_->event_loop;
            auto data_copy = data_;

            loop.schedule([data_copy, setup_rejection]() {
                setup_rejection(std::get<std::exception_ptr>(data_copy->value));
            });
        } else {
            auto data_copy = data_;
            data_->on_fulfill = [data_copy, setup_fulfillment](const T &value) {
                auto &loop = data_copy->event_loop;
                loop.schedule([setup_fulfillment, value]() {
                    setup_fulfillment(value);
                });
            };

            data_->on_reject = [data_copy, setup_rejection](std::exception_ptr error) {
                auto &loop = data_copy->event_loop;
                loop.schedule([setup_rejection, error]() {
                    setup_rejection(error);
                });
            };
        }

        return next_promise;
    }

    /**
     * @brief Finally method - executed regardless of resolution or rejection
     *
     * @tparam Func Function type
     * @param finally_fn Function executed at the end
     * @return Promise of the same type
     */
    template<typename Func>
    EventPromise<T> finally(Func &&finally_fn) {
        auto captured_fn = std::forward<Func>(finally_fn);

        return then([captured_fn](const T &value) {
                   captured_fn();
                   return value;
               })
            .template catch_error([captured_fn](std::exception_ptr err) {
                captured_fn();
                std::rethrow_exception(err);
            });
    }

    /**
     * @brief Resolve this Promise
     * @param value Value to resolve with
     */
    void resolve(T value) {
        if (data_->state != PromiseState::Pending) return;

        data_->state = PromiseState::Fulfilled;
        detail::ValueHelper<T>::store_value(data_->value, std::move(value));

        if (data_->on_fulfill) {
            const T &stored_value = std::get<T>(data_->value);
            data_->on_fulfill(stored_value);
        }
    }

    /**
     * @brief Reject this Promise
     * @param error Error information
     */
    void reject(std::exception_ptr error) {
        if (data_->state != PromiseState::Pending) return;

        data_->state = PromiseState::Rejected;
        data_->value = error;

        if (data_->on_reject) {
            data_->on_reject(std::get<std::exception_ptr>(data_->value));
        }
    }

private:
    shared_data data_;

    // Allow access to void specialization
    template<typename U>
    friend class EventPromise;
};

// Void specialization
template<>
class EventPromise<void> {
public:
    using value_type = void;
    using data_type = detail::PromiseDataType<void>;
    using shared_data = std::shared_ptr<data_type>;

    // Resolver function type
    using resolver_func = std::function<void(std::function<void()>, std::function<void(std::exception_ptr)>)>;

    EventPromise(EventLoop &event_loop)
        : data_(std::make_shared<data_type>(event_loop)) {}

    EventPromise(EventLoop &event_loop, resolver_func executor)
        : EventPromise(event_loop) {
        try {
            // Capture data_ instead of this pointer (safe because it's a shared_ptr)
            auto data = data_;
            executor(
                [data]() {
                    if (data->state != PromiseState::Pending) return;
                    data->state = PromiseState::Fulfilled;
                    if (data->on_fulfill) {
                        data->on_fulfill();
                    }
                },
                [data](std::exception_ptr err) {
                    if (data->state != PromiseState::Pending) return;
                    data->state = PromiseState::Rejected;
                    data->value = err;
                    if (data->on_reject) {
                        data->on_reject(std::get<std::exception_ptr>(data->value));
                    }
                });
        } catch (...) {
            auto data = data_;
            if (data->state == PromiseState::Pending) {
                data->state = PromiseState::Rejected;
                data->value = std::current_exception();
                if (data->on_reject) {
                    data->on_reject(std::get<std::exception_ptr>(data_->value));
                }
            }
        }
    }

    static EventPromise<void> resolve(EventLoop &event_loop) {
        EventPromise<void> promise(event_loop);
        promise.resolve();
        return promise;
    }

    static EventPromise<void> reject(EventLoop &event_loop, std::exception_ptr error) {
        EventPromise<void> promise(event_loop);
        promise.reject(std::move(error));
        return promise;
    }

    template<typename Func>
    auto then(Func &&on_fulfilled) {
        using raw_result_type = decltype(on_fulfilled());

        // If callback returns a Promise, use its value_type; otherwise use the raw result type
        using result_type = typename value_type_or_self<raw_result_type>::type;

        using next_type = typename std::conditional<
            std::is_void<result_type>::value,
            void,
            result_type>::type;

        EventPromise<next_type> next_promise(data_->event_loop);

        // Create copies of variables to capture (for C++17 compatibility)
        auto next_data = next_promise.data_;
        auto fulfilled_func = std::forward<Func>(on_fulfilled);

        auto setup_fulfillment = [next_data, fulfilled_func]() {
            try {
                if constexpr (std::is_void<raw_result_type>::value) {
                    // Void return type
                    detail::ValueHelper<void>::invoke_with_value(fulfilled_func);
                    next_data->state = PromiseState::Fulfilled;
                    if (next_data->on_fulfill) {
                        next_data->on_fulfill();
                    }
                } else if constexpr (is_event_promise<raw_result_type>::value) {
                    // Promise return type - unwrap it
                    auto inner_promise = detail::ValueHelper<void>::invoke_with_value(fulfilled_func);

                    // Connect the inner promise to the outer one
                    if (inner_promise.data_->state == PromiseState::Fulfilled) {
                        if constexpr (std::is_void<result_type>::value) {
                            // Inner promise resolves to void
                            next_data->state = PromiseState::Fulfilled;
                            if (next_data->on_fulfill) {
                                next_data->on_fulfill();
                            }
                        } else {
                            // Inner promise resolves to a value
                            next_data->state = PromiseState::Fulfilled;
                            detail::ValueHelper<next_type>::store_value(
                                next_data->value,
                                std::get<typename raw_result_type::value_type>(inner_promise.data_->value));
                            if (next_data->on_fulfill) {
                                next_data->on_fulfill(std::get<next_type>(next_data->value));
                            }
                        }
                    } else if (inner_promise.data_->state == PromiseState::Rejected) {
                        // Forward rejection
                        next_data->state = PromiseState::Rejected;
                        next_data->value = std::get<std::exception_ptr>(inner_promise.data_->value);
                        if (next_data->on_reject) {
                            next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                        }
                    } else {
                        // Set up callbacks on the inner promise
                        auto inner_data = inner_promise.data_;

                        if constexpr (std::is_void<result_type>::value) {
                            inner_data->on_fulfill = [next_data]() {
                                next_data->state = PromiseState::Fulfilled;
                                if (next_data->on_fulfill) {
                                    next_data->on_fulfill();
                                }
                            };
                        } else {
                            inner_data->on_fulfill = [next_data](const typename raw_result_type::value_type &inner_value) {
                                next_data->state = PromiseState::Fulfilled;
                                detail::ValueHelper<next_type>::store_value(next_data->value, inner_value);
                                if (next_data->on_fulfill) {
                                    next_data->on_fulfill(std::get<next_type>(next_data->value));
                                }
                            };
                        }

                        inner_data->on_reject = [next_data](std::exception_ptr error) {
                            next_data->state = PromiseState::Rejected;
                            next_data->value = error;
                            if (next_data->on_reject) {
                                next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                            }
                        };
                    }
                } else {
                    // Normal value return type
                    auto next_value = detail::ValueHelper<void>::invoke_with_value(fulfilled_func);
                    next_data->state = PromiseState::Fulfilled;
                    detail::ValueHelper<next_type>::store_value(next_data->value, std::move(next_value));
                    if (next_data->on_fulfill) {
                        next_data->on_fulfill(std::get<next_type>(next_data->value));
                    }
                }
            } catch (...) {
                next_data->state = PromiseState::Rejected;
                next_data->value = std::current_exception();
                if (next_data->on_reject) {
                    next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                }
            }
        };

        auto setup_rejection = [next_data](std::exception_ptr error) {
            next_data->state = PromiseState::Rejected;
            next_data->value = error;
            if (next_data->on_reject) {
                next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
            }
        };

        if (data_->state == PromiseState::Fulfilled) {
            auto &loop = data_->event_loop;
            loop.schedule(setup_fulfillment);
        } else if (data_->state == PromiseState::Rejected) {
            auto &loop = data_->event_loop;
            auto data_copy = data_;

            loop.schedule([data_copy, setup_rejection]() {
                setup_rejection(std::get<std::exception_ptr>(data_copy->value));
            });
        } else {
            // Eliminate dependency on this pointer
            auto data_copy = data_;
            data_->on_fulfill = [data_copy, setup_fulfillment]() {
                auto &loop = data_copy->event_loop;
                loop.schedule(setup_fulfillment);
            };

            data_->on_reject = [data_copy, setup_rejection](std::exception_ptr error) {
                auto &loop = data_copy->event_loop;
                loop.schedule([setup_rejection, error]() {
                    setup_rejection(error);
                });
            };
        }

        return next_promise;
    }

    template<typename Func>
    auto catch_error(Func &&on_rejected) {
        using raw_result_type = decltype(on_rejected(std::declval<std::exception_ptr>()));

        // If callback returns a Promise, use its value_type; otherwise use the raw result type
        using result_type = typename value_type_or_self<raw_result_type>::type;

        using next_type = typename std::conditional<
            std::is_void<result_type>::value,
            void,
            result_type>::type;

        EventPromise<next_type> next_promise(data_->event_loop);

        // Create copies of variables to capture (for C++17 compatibility)
        auto next_data = next_promise.data_;
        auto rejected_func = std::forward<Func>(on_rejected);

        auto setup_fulfillment = [next_data]() {
            next_data->state = PromiseState::Fulfilled;
            if (next_data->on_fulfill) {
                next_data->on_fulfill();
            }
        };

        auto setup_rejection = [next_data, rejected_func](std::exception_ptr error) {
            try {
                if constexpr (std::is_void<result_type>::value) {
                    // void型を返す場合
                    rejected_func(error);
                    next_data->state = PromiseState::Fulfilled;
                    if (next_data->on_fulfill) {
                        next_data->on_fulfill();
                    }
                } else if constexpr (is_event_promise<raw_result_type>::value) {
                    // Promise型を返す場合はフラット化
                    auto inner_promise = rejected_func(error);

                    // 内部Promiseの状態に応じた処理
                    if (inner_promise.data_->state == PromiseState::Fulfilled) {
                        if constexpr (std::is_void<typename raw_result_type::value_type>::value) {
                            // 内部Promiseがvoidを解決する場合
                            next_data->state = PromiseState::Fulfilled;
                            if (next_data->on_fulfill) {
                                next_data->on_fulfill();
                            }
                        } else {
                            // 内部Promiseが値を解決する場合
                            next_data->state = PromiseState::Fulfilled;
                            detail::ValueHelper<next_type>::store_value(
                                next_data->value,
                                std::get<typename raw_result_type::value_type>(inner_promise.data_->value));
                            if (next_data->on_fulfill) {
                                next_data->on_fulfill(std::get<next_type>(next_data->value));
                            }
                        }
                    } else if (inner_promise.data_->state == PromiseState::Rejected) {
                        // 内部Promiseの拒否を伝播
                        next_data->state = PromiseState::Rejected;
                        next_data->value = std::get<std::exception_ptr>(inner_promise.data_->value);
                        if (next_data->on_reject) {
                            next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                        }
                    } else {
                        // 内部Promiseがまだ解決されていない場合はコールバックを設定
                        auto inner_data = inner_promise.data_;

                        if constexpr (std::is_void<typename raw_result_type::value_type>::value) {
                            inner_data->on_fulfill = [next_data]() {
                                next_data->state = PromiseState::Fulfilled;
                                if (next_data->on_fulfill) {
                                    next_data->on_fulfill();
                                }
                            };
                        } else {
                            inner_data->on_fulfill = [next_data](const typename raw_result_type::value_type &inner_value) {
                                next_data->state = PromiseState::Fulfilled;
                                detail::ValueHelper<next_type>::store_value(next_data->value, inner_value);
                                if (next_data->on_fulfill) {
                                    next_data->on_fulfill(std::get<next_type>(next_data->value));
                                }
                            };
                        }

                        inner_data->on_reject = [next_data](std::exception_ptr inner_error) {
                            next_data->state = PromiseState::Rejected;
                            next_data->value = inner_error;
                            if (next_data->on_reject) {
                                next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                            }
                        };
                    }
                } else {
                    // 通常の値を返す場合
                    auto recovery_value = rejected_func(error);
                    next_data->state = PromiseState::Fulfilled;
                    detail::ValueHelper<next_type>::store_value(next_data->value, std::move(recovery_value));
                    if (next_data->on_fulfill) {
                        next_data->on_fulfill(std::get<next_type>(next_data->value));
                    }
                }
            } catch (...) {
                next_data->state = PromiseState::Rejected;
                next_data->value = std::current_exception();
                if (next_data->on_reject) {
                    next_data->on_reject(std::get<std::exception_ptr>(next_data->value));
                }
            }
        };

        if (data_->state == PromiseState::Fulfilled) {
            auto &loop = data_->event_loop;
            loop.schedule(setup_fulfillment);
        } else if (data_->state == PromiseState::Rejected) {
            auto &loop = data_->event_loop;
            auto data_copy = data_;

            loop.schedule([data_copy, setup_rejection]() {
                setup_rejection(std::get<std::exception_ptr>(data_copy->value));
            });
        } else {
            auto data_copy = data_;
            data_->on_fulfill = [data_copy, setup_fulfillment]() {
                auto &loop = data_copy->event_loop;
                loop.schedule(setup_fulfillment);
            };

            data_->on_reject = [data_copy, setup_rejection](std::exception_ptr error) {
                auto &loop = data_copy->event_loop;
                loop.schedule([setup_rejection, error]() {
                    setup_rejection(error);
                });
            };
        }

        return next_promise;
    }

    template<typename Func>
    EventPromise<void> finally(Func &&finally_fn) {
        auto captured_fn = std::forward<Func>(finally_fn);

        return then([captured_fn]() {
                   captured_fn();
               })
            .template catch_error([captured_fn](std::exception_ptr err) {
                captured_fn();
                std::rethrow_exception(err);
            });
    }

    void resolve() {
        if (data_->state != PromiseState::Pending) return;

        data_->state = PromiseState::Fulfilled;

        if (data_->on_fulfill) {
            data_->on_fulfill();
        }
    }

    void reject(std::exception_ptr error) {
        if (data_->state != PromiseState::Pending) return;

        data_->state = PromiseState::Rejected;
        data_->value = error;

        if (data_->on_reject) {
            data_->on_reject(std::get<std::exception_ptr>(data_->value));
        }
    }

private:
    shared_data data_;

    template<typename U>
    friend class EventPromise;
};

/**
 * @brief Helper extension class for EventLoop - Promise-based interface
 */
class EventPromiseFactory {
public:
    EventPromiseFactory(EventLoop &event_loop): event_loop_(event_loop) {}

    /**
     * @brief Create immediately resolved Promise
     */
    template<typename T>
    EventPromise<T> resolve(T value) {
        return EventPromise<T>::resolve(event_loop_, std::move(value));
    }

    /**
     * @brief Resolve for void type
     */
    EventPromise<void> resolve() {
        return EventPromise<void>::resolve(event_loop_);
    }

    /**
     * @brief Create immediately rejected Promise
     */
    template<typename T = void>
    EventPromise<T> reject(std::exception_ptr error) {
        return EventPromise<T>::reject(event_loop_, std::move(error));
    }

    /**
     * @brief Create rejection from exception
     */
    template<typename E, typename T = void>
    EventPromise<T> reject(const E &exception) {
        try {
            throw exception;
        } catch (...) {
            return reject<T>(std::current_exception());
        }
    }

    /**
     * @brief Create Promise from asynchronous task
     */
    template<typename T>
    EventPromise<T> create(typename EventPromise<T>::resolver_func executor) {
        return EventPromise<T>(event_loop_, std::move(executor));
    }

    /**
     * @brief Create asynchronous task for void type
     */
    EventPromise<void> create_void(typename EventPromise<void>::resolver_func executor) {
        return EventPromise<void>(event_loop_, std::move(executor));
    }

    /**
     * @brief Create delayed execution Promise
     */
    auto delay(std::chrono::milliseconds duration) {
        return create_void([this, duration](auto resolve, auto reject) {
            event_loop_.schedule([resolve, reject]() {
                try {
                    resolve();

                } catch (...) {
                    reject(std::current_exception());
                }
            },
                                 duration);
        });
    }

private:
    EventLoop &event_loop_;
};

} // namespace nodec::asyncio

#endif