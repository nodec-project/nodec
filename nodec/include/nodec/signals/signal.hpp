#ifndef NODEC__SIGNALS__SIGNAL_HPP_
#define NODEC__SIGNALS__SIGNAL_HPP_

#include "details.hpp"

namespace nodec {
namespace signals {

/**
 * @note
 *   This code is based on the followings. Thank you!
 *   * <https://github.com/TheWisp/signals>
 */

/**
 * @brief Represents a unmanaged (raw) connection.
 *
 * @warning
 *   Do not hold this instance for a long time.
 *   If you want to use a connection, you should assign it to the connection object
 *   as soon as possible.
 */
struct RawConnection {
    RawConnection(details::BaseConnectionImpl *impl)
        : ptr(impl) {}

    RawConnection(const RawConnection &) = delete;
    RawConnection &operator=(const RawConnection &) = delete;

    //! Do not use this directly.
    //! Make the Connection object.
    details::BaseConnectionImpl *ptr;
};

class Connection {
public:
    Connection()
        : impl_(nullptr) {}

    Connection(RawConnection &&raw) noexcept {
        assert(raw.ptr && "You may attempted to make connection from an already moved raw connection.");

        impl_ = raw.ptr;
        raw.ptr = nullptr;

        impl_->mark_owned();
    }

    Connection(Connection &&other) noexcept
        : impl_(other.impl_) {
        other.impl_ = nullptr;
    }

    ~Connection() {
        disconnect();
    }

    Connection(const Connection &) = delete;
    Connection &operator=(const Connection &) = delete;

    Connection &operator=(Connection &&other) noexcept {
        disconnect();
        impl_ = other.impl_;
        other.impl_ = nullptr;
        return *this;
    }

    Connection &operator=(RawConnection &&raw) noexcept {
        assert(raw.ptr && "You may attempted to make connection from an already moved raw connection.");

        disconnect();
        impl_ = raw.ptr;
        raw.ptr = nullptr;
        impl_->mark_owned();
        return *this;
    }

    void disconnect() {
        delete impl_;
        impl_ = nullptr;
    }

    void block() {
        if (!impl_) return;
        impl_->block();
    }

    void unblock() {
        if (!impl_) return;
        impl_->unblock();
    }

private:
    details::BaseConnectionImpl *impl_;
};

template<typename>
class SignalInterface;

template<typename... Args>
class SignalInterface<void(Args...)> final {
    using SignalImpl = details::SignalImpl<void(Args...)>;

public:
    SignalInterface(SignalImpl &impl)
        : impl_(impl) {}

    SignalInterface(const SignalInterface &) = delete;
    SignalInterface &operator=(const SignalInterface &) = delete;

    SignalInterface(SignalInterface &&other) noexcept
        : impl_(other.impl_) {}

    template<class Func>
    RawConnection connect(Func &&functor) {
        auto *connection = impl_.connect(std::forward<Func>(functor));
        return {connection};
    }

private:
    SignalImpl &impl_;
};

template<typename>
class Signal;

template<typename... Args>
class Signal<void(Args...)> final {
public:
    using SignalInterface = SignalInterface<void(Args...)>;

    Signal() {}
    ~Signal() {}

    Signal(Signal &&other) noexcept
        : impl_(std::move(impl_)) {
    }

    Signal &operator=(Signal &&other) noexcept {
        impl_ = std::move(impl_);
        return *this;
    }

    SignalInterface signal_interface() {
        return {impl_};
    }

    void operator()(Args... args) {
        impl_.operator()(std::forward<Args>(args)...);
    }

private:
    details::SignalImpl<void(Args...)> impl_;
};

} // namespace signals
} // namespace nodec

#endif