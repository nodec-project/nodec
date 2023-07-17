#ifndef NODEC__SIGNALS__SCOPED_BLOCK_HPP_
#define NODEC__SIGNALS__SCOPED_BLOCK_HPP_

namespace nodec {
namespace signals {

template<typename Connection>
class ScopedBlock {
public:
    /**
     * @brief Construct and block
     */
    explicit ScopedBlock(Connection &conn)
        : conn(conn) {
        conn.block();
    }

    ~ScopedBlock() noexcept {
        conn.unblock();
    }

    ScopedBlock(const ScopedBlock &) = delete;
    ScopedBlock &operator=(const ScopedBlock &) = delete;

private:
    Connection &conn;
};

} // namespace signals
} // namespace nodec

#endif
