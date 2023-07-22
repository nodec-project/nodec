#ifndef NODEC__FLAGS_HPP_
#define NODEC__FLAGS_HPP_

/**
 * https://github.com/grisumbras/enum-flags
 */
#include <bitset>
#include <ostream>
#include <type_traits>

#include "flags_iterator.hpp"
#include "type_traits.hpp"

namespace nodec {

template<class E, class Enabler = void>
struct is_flags : public std::false_type {
};

template<class E>
struct is_flags<E, void_t<decltype(E::_nodec_flags_enable)>> : std::is_enum<E> {};

} // namespace nodec

#define NODEC_FLAGS_ENABLE(name) \
    namespace nodec { \
    template<> \
    struct is_flags<name> : std::true_type {}; \
    }

namespace nodec {

template<class E>
class Flags {
public:
    static_assert(is_flags<E>::value,
                  "nodec is disallowed for this type; "
                  "use NODEC_ALLOW_FLAGS_FOR_ENUM macro.");

    using enum_type = typename std::decay<E>::type;
    using underlying_type = typename std::underlying_type<enum_type>::type;
    using impl_type = typename std::make_unsigned<underlying_type>::type;

    using iterator = FlagsIterator<enum_type>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;
    using reference = typename iterator::reference;
    using const_reference = typename iterator::reference;
    using pointer = enum_type *;
    using const_pointer = const enum_type *;
    using size_type = std::size_t;
    using difference_type = typename iterator::difference_type;

    constexpr static std::size_t size() {
        return sizeof(impl_type) * 8;
    }

public:
    Flags(const Flags &) noexcept = default;
    Flags &operator=(const Flags &) noexcept = default;
    Flags(Flags &&) noexcept = default;
    Flags &operator=(Flags &&) noexcept = default;

    constexpr Flags() noexcept
        : val_(0) {}

    constexpr Flags(enum_type e) noexcept
        : val_(static_cast<impl_type>(e)) {
    }

    Flags &operator=(enum_type e) noexcept {
        val_ = static_cast<impl_type>(e);
        return *this;
    }

    constexpr explicit operator bool() const noexcept {
        return val_ != 0;
    }

    constexpr bool operator!() const noexcept {
        return !val_;
    }

    friend constexpr bool operator==(Flags fl1, Flags fl2) {
        return fl1.val_ == fl2.val_;
    }

    friend constexpr bool operator!=(Flags fl1, Flags fl2) {
        return fl1.val_ != fl2.val_;
    }

    constexpr Flags operator~() const noexcept {
        return Flags(~val_);
    }

    Flags &operator|=(const Flags &fl) noexcept {
        val_ |= fl.val_;
        return *this;
    }

    Flags &operator&=(const Flags &fl) noexcept {
        val_ &= fl.val_;
        return *this;
    }

    Flags &operator^=(const Flags &fl) noexcept {
        val_ ^= fl.val_;
        return *this;
    }

    Flags &operator|=(enum_type e) noexcept {
        val_ |= static_cast<impl_type>(e);
        return *this;
    }

    Flags &operator&=(enum_type e) noexcept {
        val_ &= static_cast<impl_type>(e);
        return *this;
    }

    Flags &operator^=(enum_type e) noexcept {
        val_ ^= static_cast<impl_type>(e);
        return *this;
    }

    friend constexpr Flags operator|(Flags f1, Flags f2) noexcept {
        return Flags{static_cast<impl_type>(f1.val_ | f2.val_)};
    }

    friend constexpr Flags operator&(Flags f1, Flags f2) noexcept {
        return Flags{static_cast<impl_type>(f1.val_ & f2.val_)};
    }

    friend constexpr Flags operator^(Flags f1, Flags f2) noexcept {
        return Flags{static_cast<impl_type>(f1.val_ ^ f2.val_)};
    }

    constexpr underlying_type underlying_value() const noexcept {
        return static_cast<underlying_type>(val_);
    }

    void set_underlying_value(underlying_type value) noexcept {
        val_ = static_cast<impl_type>(value);
    }

    constexpr bool none() const noexcept {
        return !val_;
    }

    void clear() noexcept {
        val_ = 0;
    }

    iterator begin() const noexcept {
        return cbegin();
    }

    iterator cbegin() const noexcept {
        return iterator{val_};
    }

    constexpr iterator end() const noexcept {
        return cend();
    }
    constexpr iterator cend() const noexcept {
        return {};
    }

private:
    constexpr explicit Flags(impl_type val) noexcept
        : val_(val) {
    }

    impl_type val_;
};

template<class E>
auto operator<<(std::ostream &stream, const nodec::Flags<E> &flags)
    -> typename std::enable_if<
        nodec::can_stream_out<std::ostream, E>::value, std::ostream &>::type {
    auto iter = flags.begin();
    if (iter == flags.end()) {
        return stream << "None";
    }

    stream << *iter;
    while (++iter != flags.end()) {
        stream << "|" << *iter;
    }

    // std::copy(flags.begin(), flags.end(), std::ostream_iterator<E>(stream, "|"));

    return stream;
}

template<class E>
auto operator<<(std::ostream &stream, const nodec::Flags<E> &flags)
    -> typename std::enable_if<
        !nodec::can_stream_out<std::ostream, E>::value, std::ostream &>::type {
    return stream << std::bitset<flags.size()>(flags.underlying_value())
                  << "(" << std::showbase << std::hex << flags.underlying_value() << ")";
}

} // namespace nodec

template<class E>
constexpr auto operator|(E e1, E e2) noexcept
    -> typename std::enable_if<
        nodec::is_flags<E>::value,
        nodec::Flags<E>>::type {
    return nodec::Flags<E>(e1) | e2;
}

template<class E>
constexpr auto operator&(E e1, E e2) noexcept
    -> typename std::enable_if<
        nodec::is_flags<E>::value,
        nodec::Flags<E>>::type {
    return nodec::Flags<E>(e1) & e2;
}

template<class E>
constexpr auto operator^(E e1, E e2) noexcept
    -> typename std::enable_if<
        nodec::is_flags<E>::value,
        nodec::Flags<E>>::type {
    return nodec::Flags<E>(e1) ^ e2;
}

template<class E>
constexpr auto operator~(E e1) noexcept
    -> typename std::enable_if<
        nodec::is_flags<E>::value,
        nodec::Flags<E>>::type {
    return ~nodec::Flags<E>(e1);
}

#endif
