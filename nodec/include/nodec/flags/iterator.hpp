#ifndef NODEC__FLAGS__ITERATOR_HPP_
#define NODEC__FLAGS__ITERATOR_HPP_

#include <iterator>

namespace nodec {
namespace flags {

template<class E>
class Flags;

template<class E>
class FlagsIterator {
public:
    using Flags = Flags<E>;
    using impl_type = typename Flags::impl_type;

    using difference_type = std::ptrdiff_t;
    using value_type = E;
    using pointer = value_type *;
    using reference = const value_type;
    using iterator_category = std::forward_iterator_tag;

    constexpr FlagsIterator() noexcept
        : uvalue_(0), mask_(0) {
    }

    constexpr FlagsIterator(const FlagsIterator &other) noexcept
        : uvalue_(other.uvalue_), mask_(other.mask_) {
    }

    explicit FlagsIterator(impl_type uv) noexcept
        : uvalue_(uv), mask_(0x01) {
        if (!(mask_ & uvalue_)) {
            next();
        }
    }

    FlagsIterator &operator++() noexcept {
        next();
        return *this;
    }

    FlagsIterator &operator++(int) noexcept {
        auto copy = *this;
        ++(*this);
        return copy;
    }

    constexpr reference operator*() const noexcept {
        return static_cast<value_type>(mask_);
    }

    friend inline constexpr bool operator==(const FlagsIterator &i,
                                            const FlagsIterator &j) noexcept {
        return i.mask_ == j.mask_;
    }

    friend inline constexpr bool operator!=(const FlagsIterator &i,
                                            const FlagsIterator &j) noexcept {
        return i.mask_ != j.mask_;
    }

private:
    void next() noexcept {
        do {
            mask_ <<= 1;
        } while (mask_ && !(mask_ & uvalue_));
    }

    impl_type uvalue_;
    impl_type mask_;
};

} // namespace flags
} // namespace nodec

#endif