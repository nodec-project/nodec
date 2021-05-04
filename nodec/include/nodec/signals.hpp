#ifndef NODEC__SIGNALS_HPP_
#define NODEC__SIGNALS_HPP_

#include <nodec/error_formatter.hpp>

#include <functional>
#include <vector>
#include <stdexcept>

namespace nodec {

/**
* @note
*   This code is based on
*   * <https://github.com/TheWisp/signals>
*/
namespace signals {

namespace details {

inline void throw_broken_connection_exception(const char* file, size_t line) {
    throw std::logic_error(error_fomatter::type_file_line<std::logic_error>(
        "This connection is broken.",
        file, line
        ));
}

template<typename> class BaseSignal;
template<typename... As>
class BaseSignal<void(As...)> {
public:
    using Callback = std::function<void(As...)>;

    struct BlockedConnection {
        BaseSignal* sig{ nullptr };
        Callback call{ nullptr };
    };

    class BaseConnection {
    public:
        bool owned{ false };
        size_t idx;

        BaseConnection(BaseSignal* sig, size_t idx) : sig(sig), idx(idx) {}
        ~BaseConnection() {
            if (blocked) {
                delete blocked_conn;
            }
            else {
                if (sig) {
                    sig->calls[idx] = nullptr;
                    sig->conns[idx] = nullptr;
                    sig->dirty = true;
                }
            }
        }

        void set_sig(BaseSignal* sig) {
            if (blocked) {
                this->blocked_conn->sig = sig;
            }
            else {
                this->sig = sig;
            }
        }

        void block() {
            if (blocked) {
                return;
            };

            blocked = true;
            BaseSignal* orig_sig = sig;
            sig = nullptr;
            blocked_conn = new BlockedConnection();
            blocked_conn->sig = orig_sig;
            std::swap(blocked_conn->call, orig_sig->calls[idx]);
        }

        void unblock() {
            if (!blocked) {
                return;
            }

            BaseSignal* orig_sig = blocked_conn->sig;
            std::swap(blocked_conn->call, orig_sig->calls[idx]);
            delete blocked_conn;
            blocked_conn = nullptr;
            sig = orig_sig;
            blocked = false;
        }

    private:
        union {
            BaseSignal* sig;
            BlockedConnection* blocked_conn;
        };

        bool blocked{ false };
    };


public:
    BaseSignal() = default;
    ~BaseSignal() {
        for (BaseConnection* c : conns) {
            if (c) {
                if (c->owned) {
                    c->set_sig(nullptr);
                }
                else {
                    delete c;
                }
            }
        }
    }

    BaseSignal(const BaseSignal&) = delete;
    BaseSignal& operator= (const BaseSignal&) = delete;

    BaseSignal(BaseSignal&& other) noexcept
        : calls(std::move(other.calls))
        , conns(std::move(other.conns))
        , dirty(other.dirty) {
        for (BaseConnection* c : conns) {
            if (c) {
                c->set_sig(this);
            }
        }
    }

    BaseSignal& operator= (BaseSignal&& other) noexcept {
        calls = std::move(other.calls);
        conns = std::move(other.conns);
        dirty = other.dirty;
        return *this;
    }

public:
    std::vector<Callback> calls;
    std::vector<BaseConnection*> conns;

    bool dirty{ false };
};

}


template<typename> class SignalInterface;
template<typename... As>
class SignalInterface<void(As...)> {
    using BaseSignal = details::BaseSignal<void(As...)>;
    using BaseConnection = typename BaseSignal::BaseConnection;

public:

    class Connection;

    /**
    * @brief A connection without auto disconnection.
    */
    class RawConnection {
    public:
        RawConnection(BaseConnection* ptr)
            :ptr(ptr) {
        }

        Connection lock() const {
            return { ptr };
        }

        operator Connection() const {
            return { ptr };
        }
    private:
        BaseConnection* ptr;
    };

    class Connection {
    public:
        Connection(BaseConnection* ptr)
            :ptr(ptr) {
            ptr->owned = true;
        }

        ~Connection() {
            disconnect();
        }

        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        Connection(Connection&& other) noexcept
            : ptr(other.ptr) {
            other.ptr = nullptr;
        }

        Connection& operator=(Connection&& other) noexcept {
            disconnect(); // free ptr
            ptr = other.ptr;
            other.ptr = nullptr;
            return *this;
        }


        void disconnect() {
            delete ptr;
            ptr = nullptr;
        }

        void block() {
            if (!ptr) {
                details::throw_broken_connection_exception(__FILE__, __LINE__);
            }

            ptr->block();
        }

        void unblock() {
            if (!ptr) {
                details::throw_broken_connection_exception(__FILE__, __LINE__);
            }

            ptr->unblock();
        }


    private:
        BaseConnection* ptr;
    };



public:
    SignalInterface() = default;

    SignalInterface(SignalInterface&& other) = delete;
    SignalInterface& operator= (SignalInterface&& other) = delete;

    SignalInterface(const BaseSignal&) = delete;
    SignalInterface& operator= (const BaseSignal&) = delete;

public:
    template<class F>
    RawConnection connect(F&& functor) {
        size_t idx = base_sig.conns.size();
        base_sig.calls.emplace_back(functor);
        BaseConnection* conn = new BaseConnection(&base_sig, idx);
        base_sig.conns.emplace_back(conn);
        return { conn };
    }

protected:
    BaseSignal base_sig;
};

template<typename> class Signal;
template<typename... As>
class Signal<void(As...)> : public SignalInterface<void(As...)> {
    using BaseSignal = details::BaseSignal<void(As...)>;
    using BaseConnection = typename BaseSignal::BaseConnection;

public:
    using Interface = SignalInterface<void(As...)>;

public:
    Signal() = default;

    Signal(Signal&& other) noexcept
        : Interface::base_sig(std::move(other.base_sig))
        , calling(other.calling) {
    }

    Signal& operator= (Signal&& other) noexcept {
        Interface::base_sig = std::move(other.base_sig);
        calling = other.calling;
        return *this;
    }

    void operator()(As&&... args) {
        bool recursion = calling;
        calling = true;

        for (size_t i = 0, n = Interface::base_sig.calls.size(); i < n; ++i) {
            auto& cb = Interface::base_sig.calls[i];
            if (cb) {
                cb(std::forward<As>(args)...);
            }
        }

        if (!recursion) {
            calling = false;

        }

    }

    Interface& interface() {
        return *this;
    }

private:
    bool calling{ false };


};


}
}


#endif