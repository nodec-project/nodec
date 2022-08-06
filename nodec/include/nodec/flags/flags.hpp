#ifndef NODEC__FLAGS__FLAGS_HPP_
#define NODEC__FLAGS__FLAGS_HPP_

#include "allow_flags.hpp"
#include "iterator.hpp"

#include <nodec/type_traits.hpp>

#include <bitset>
#include <ostream>
#include <type_traits>

namespace nodec {
namespace flags {

constexpr struct Empty {
    constexpr Empty() noexcept = default;
} empty;

template<class E>
class Flags {
public:
    static_assert(is_flags<E>::value,
                  "flags::flags is disallowed for this type; "
                  "use ALLOW_FLAGS_FOR_ENUM macro.");

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
    Flags() noexcept = default;
    Flags(const Flags &) noexcept = default;
    Flags &operator=(const Flags &) noexcept = default;
    Flags(Flags &&) noexcept = default;
    Flags &operator=(Flags &&) noexcept = default;

    explicit constexpr Flags(Empty) noexcept
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

    // template <class E>
    // friend constexpr auto operator<<(std::ostream& stream, const Flags<E>& fl)
    //     -> typename std::enable_if<
    //     nodec::is_printable<E>::value, std::ostream&
    //     >::type {
    //     return stream << "printable. ";
    // }

    // template <class E>
    // friend constexpr auto operator<<(std::ostream& stream, const Flags<E>& fl)
    //     -> typename std::enable_if<
    //     !nodec::is_printable<E>::value, std::ostream&
    //     >::type {
    //     return stream << "not printable. ";
    // }

    // friend auto operator<<(std::ostream& stream, const Flags& fl)
    //     -> typename std::enable_if<
    //     nodec::is_printable<E>::value, std::ostream&
    //     >::type {
    //     return stream << "printable. ";
    // }

    // friend auto operator<<(std::ostream& stream, const Flags& fl)
    //     -> typename std::enable_if<
    //     !nodec::is_printable<E>::value, std::ostream&
    //     >::type {
    //     return stream << "not printable. ";
    // }

    // friend constexpr auto operator<<(std::ostream& stream, const Flags& fl)
    //     -> typename std::enable_if<
    //     nodec::is_printable<enum_type>::value, std::ostream&
    //     >::type {
    //     return stream << "printable. " << fl.val_;
    // }

    // friend auto operator<<(std::ostream& stream, const Flags& fl)
    //     -> typename std::enable_if<
    //     !nodec::is_printable<enum_type>::value, std::ostream&
    //     >::type {
    //     return stream << "not printable. " << fl.val_;
    // }

    // friend constexpr auto operator<<(std::ostream& stream, const Flags& fl)
    //     -> typename std::enable_if<
    //     nodec::is_printable<enum_type>::value,
    //     std::ostream&>::type {
    //     return stream << fl.val_;
    // }

private:
    constexpr explicit Flags(impl_type val) noexcept
        : val_(val) {
    }

    impl_type val_;
};

} // namespace flags
} // namespace nodec

template<class E>
constexpr auto operator|(E e1, E e2) noexcept
    -> typename std::enable_if<
        nodec::flags::is_flags<E>::value,
        nodec::flags::Flags<E>>::type {
    return nodec::flags::Flags<E>(e1) | e2;
}

template<class E>
constexpr auto operator&(E e1, E e2) noexcept
    -> typename std::enable_if<
        nodec::flags::is_flags<E>::value,
        nodec::flags::Flags<E>>::type {
    return nodec::flags::Flags<E>(e1) & e2;
}

template<class E>
constexpr auto operator^(E e1, E e2) noexcept
    -> typename std::enable_if<
        nodec::flags::is_flags<E>::value,
        nodec::flags::Flags<E>>::type {
    return nodec::flags::Flags<E>(e1) ^ e2;
}

template<class E>
constexpr auto operator~(E e1) noexcept
    -> typename std::enable_if<
        nodec::flags::is_flags<E>::value,
        nodec::flags::Flags<E>>::type {
    return ~nodec::flags::Flags<E>(e1);
}

//
// template <class E>
// auto print(std::ostream& stream, const nodec::flags::Flags<E>& fl)
//-> typename std::enable_if<
//    nodec::is_printable<E>::value, std::ostream&
//>::type {
//    return stream << "printable. ";
//}
//
// template <class E>
// auto print(std::ostream& stream, const nodec::flags::Flags<E>& fl)
//-> typename std::enable_if<
//    !nodec::is_printable<E>::value, std::ostream&
//>::type {
//    return stream << "not printable. ";
//}

template<class E>
auto operator<<(std::ostream &stream, const nodec::flags::Flags<E> &flags)
    -> typename std::enable_if<
        nodec::is_printable<E>::value, std::ostream &>::type {
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
auto operator<<(std::ostream &stream, const nodec::flags::Flags<E> &flags)
    -> typename std::enable_if<
        !nodec::is_printable<E>::value, std::ostream &>::type {
    return stream << std::bitset<flags.size()>(flags.underlying_value())
                  << "(" << std::showbase << std::hex << flags.underlying_value() << ")";
}

#endif
