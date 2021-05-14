#ifndef NODEC__SIGNALS_HPP_
#define NODEC__SIGNALS_HPP_

#include <nodec/error_formatter.hpp>

#include <functional>
#include <vector>
#include <stdexcept>

namespace nodec {

/**
* @note
*   This code is based on the followings. Thank you!
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
template<typename... Args>
class BaseSignal<void(Args...)> {
public:
    using Callback = std::function<void(Args...)>;

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
            unblock();

            if (sig) {
                sig->calls[idx] = nullptr;
                sig->conns[idx] = nullptr;
                sig->dirty = true;
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
        for (BaseConnection* c : conns) {
            if (c) {
                c->set_sig(this);
            }
        }
        return *this;
    }

public:
    std::vector<Callback> calls;
    std::vector<BaseConnection*> conns;

    bool dirty{ false };
};

} // namespace details


template<typename> class SignalInterface;
template<typename... Args>
class SignalInterface<void(Args...)> {
    using BaseSignal = details::BaseSignal<void(Args...)>;
    using BaseConnection = typename BaseSignal::BaseConnection;

public:

    class Connection;

    /**
    * @brief The temporary connection object.
    *   Operations on connections are possible only after they are assigned.
    *
    * @warning
    *   Holding the temporary object for a long time mat destroy the connection it holds.
    *   If you want to use a connection, you should assign it to the connection object
    *   as soon as possible.
    *
    */
    class TemporaryConnection {
    public:
        TemporaryConnection(BaseConnection* ptr)
            :ptr(ptr) {
        }

        TemporaryConnection(const TemporaryConnection&) = delete;
        TemporaryConnection& operator=(const TemporaryConnection&) = delete;

        TemporaryConnection(TemporaryConnection&& other) noexcept
            : ptr(other.ptr) {
            other.ptr = nullptr;
        }
        TemporaryConnection& operator=(TemporaryConnection&& other) noexcept {
            ptr = other.ptr;
            other.ptr = nullptr;
            return *this;
        }

        Connection assign() {
            auto* orig_ptr = ptr;
            ptr = nullptr;
            return { orig_ptr };
        }

    private:
        BaseConnection* ptr{ nullptr };
    };

    class Connection {
    public:
        Connection() {}

        Connection(BaseConnection* ptr)
            :ptr(ptr) {
            if (ptr) {
                ptr->owned = true;
            }
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

        Connection(TemporaryConnection&& other) noexcept {
            *this = std::move(other.assign());
        }

        Connection& operator=(Connection&& other) noexcept {
            disconnect();
            ptr = other.ptr;
            other.ptr = nullptr;
            return *this;
        }

        Connection& operator=(TemporaryConnection&& other) noexcept {
            *this = std::move(other.assign());
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
        BaseConnection* ptr{ nullptr };
    };



public:
    SignalInterface() = default;

    SignalInterface(SignalInterface&& other) = delete;
    SignalInterface& operator= (SignalInterface&& other) = delete;

    SignalInterface(const BaseSignal&) = delete;
    SignalInterface& operator= (const BaseSignal&) = delete;

public:


    template<class F>
    TemporaryConnection connect(F&& functor) {
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
template<typename... Args>
class Signal<void(Args...)> : public SignalInterface<void(Args...)> {
    using BaseSignal = details::BaseSignal<void(Args...)>;
    using BaseConnection = typename BaseSignal::BaseConnection;

public:
    using Interface = SignalInterface<void(Args...)>;

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

    //template<typename... ActualArgs>
    void operator()(Args... args) {
        bool recursion = calling;
        calling = true;

        for (size_t i = 0, n = Interface::base_sig.calls.size(); i < n; ++i) {
            auto& cb = Interface::base_sig.calls[i];
            if (cb) {
                cb(std::forward<Args>(args)...);
            }
        }

        if (!recursion) {
            calling = false;
            if (Interface::base_sig.dirty) {
                Interface::base_sig.dirty = false;
                // remove all empty slots while patching the store index in the connection.
                size_t sz = 0;
                for (size_t i = 0, n = Interface::base_sig.conns.size(); i < n; ++i) {
                    if (Interface::base_sig.conns[i]) {
                        Interface::base_sig.conns[sz] = Interface::base_sig.conns[i];
                        Interface::base_sig.calls[sz] = Interface::base_sig.calls[i];
                        Interface::base_sig.conns[sz]->idx = sz;
                        ++sz;
                    }
                }
                Interface::base_sig.conns.resize(sz);
                Interface::base_sig.calls.resize(sz);
            }
        }
    }

    Interface& interface() {
        return *this;
    }

private:
    bool calling{ false };
};

template<typename Connection>
class ScopedBlock {
public:
    /**
    * @brief Construct and block
    */
    explicit ScopedBlock(Connection& conn)
        : conn(conn) {
        conn.block();
    }

    ~ScopedBlock() noexcept {
        conn.unblock();
    }


    ScopedBlock(const ScopedBlock&) = delete;
    ScopedBlock& operator=(const ScopedBlock&) = delete;

private:
    Connection& conn;
};

} // namespace signals
}


#endif