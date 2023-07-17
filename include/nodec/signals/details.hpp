#ifndef NODEC__SIGNALS__DETAILS_HPP_
#define NODEC__SIGNALS__DETAILS_HPP_

#include <cassert>
#include <functional>
#include <vector>

namespace nodec {
namespace signals {

namespace details {

class BaseConnectionImpl {
public:
    virtual ~BaseConnectionImpl() {}

    virtual void block() = 0;
    virtual void unblock() = 0;
    virtual void mark_owned() noexcept = 0;
};

template<typename>
class SignalImpl;

template<typename... Args>
class SignalImpl<void(Args...)> final {
public:
    using Callback = std::function<void(Args...)>;

public:
    class ConnectionImpl final : public BaseConnectionImpl {
    public:
        ConnectionImpl(SignalImpl *signal, std::size_t index)
            : signal_(signal), index_(index) {}

        ~ConnectionImpl() {
            if (signal_) {
                signal_->callbacks_[index_] = nullptr;
                signal_->connections_[index_] = nullptr;
                signal_->dirty_ = true;
            }
        }

        void set_signal(SignalImpl *signal) {
            signal_ = signal;
        }

        void set_index(std::size_t index) noexcept {
            index_ = index;
        }

        void block() override {
            assert(signal_ && "Signal instance already have been deleted. The block/unblock function should be called in the callback.");

            if (swapped_callback_) return;

            std::swap(swapped_callback_, signal_->callbacks_[index_]);
        }

        void unblock() override {
            assert(signal_ && "Signal instance already have been deleted. The block/unblock function should be called in the callback.");

            if (!swapped_callback_) return;

            std::swap(swapped_callback_, signal_->callbacks_[index_]);
        }

        void mark_owned() noexcept override {
            has_owner_ = true;
        }

        bool has_owner() const noexcept {
            return has_owner_;
        }

    private:
        SignalImpl *signal_;
        std::size_t index_;

        //! This instance is owned by the client not signal.
        bool has_owner_{false};

        Callback swapped_callback_;
    };

public:
    SignalImpl() {}

    ~SignalImpl() {
        for (auto *connection : connections_) {
            if (!connection) continue;

            if (connection->has_owner()) {
                connection->set_signal(nullptr);
            } else {
                delete connection;
            }
        }
    }

    SignalImpl(const SignalImpl &) = delete;
    SignalImpl &operator=(const SignalImpl &) = delete;

    SignalImpl(SignalImpl &&other) noexcept
        : callbacks_(std::move(other.callbacks_)),
          connections_(std::move(other.connections_)),
          dirty_(other.dirty_) {
        assert(!calling_ && "You attempted to move the signal while calling. Reconsider your design.");

        for (auto *connection : connections_) {
            if (!connection) continue;
            connection->set_signal(this);
        }
    }

    SignalImpl &operator=(SignalImpl &&other) noexcept {
        assert(!calling_ && "You attempted to move the signal while calling. Reconsider your design.");

        callbacks_ = std::move(other.callbacks_);
        connections_ = std::move(other.connections_);
        dirty_ = other.dirty_;

        for (auto *connection : connections_) {
            if (!connection) continue;
            connection->set_signal(this);
        }
        return *this;
    }

public:
    template<class Func>
    ConnectionImpl *connect(Func &&functor) {
        const auto index = connections_.size();
        callbacks_.emplace_back(functor);
        auto *connection = new ConnectionImpl(this, index);
        connections_.emplace_back(connection);
        return connection;
    }

    void operator()(Args... args) {
        bool recursion = calling_;
        calling_ = true;

        for (std::size_t i = 0, n = callbacks_.size(); i < n; ++i) {
            const auto &callback = callbacks_[i];
            if (!callback) continue;
            callback(std::forward<Args>(args)...);
        }

        // --- Post process ---
        if (recursion) return;
        calling_ = false;

        if (!dirty_) return;
        dirty_ = false;

        // Remove all empty slots while patching the store index in the connection.
        size_t sz = 0;
        for (size_t i = 0, n = connections_.size(); i < n; ++i) {
            if (connections_[i] == nullptr) continue;

            connections_[sz] = connections_[i];
            callbacks_[sz] = callbacks_[i];
            connections_[sz]->set_index(sz);
            ++sz;
        }
        connections_.resize(sz);
        callbacks_.resize(sz);
    }

private:
    std::vector<Callback> callbacks_;
    std::vector<ConnectionImpl *> connections_;
    bool dirty_{false};
    bool calling_{false};
};

} // namespace details

} // namespace signals
} // namespace nodec

#endif