#ifndef NODEC__TYPE_INFO_HPP_
#define NODEC__TYPE_INFO_HPP_

#include "utility.hpp"

#include <atomic>
#include <cstdint>
#include <type_traits>

namespace nodec {

using type_seq_index_type = std::uint32_t;
// using TypeId = uint32_t;

namespace internal {

struct type_seq_index final {
    static type_seq_index_type next() noexcept {
        // Since a function is defined only once, there is only one static variable.
        static std::atomic<type_seq_index_type> value{};
        return value++;
    }
};

} // namespace internal

template<typename T>
struct type_seq_index final {
    static type_seq_index_type value() noexcept {
        static const type_seq_index_type value = internal::type_seq_index::next();
        return value;
    }
};

struct type_info final {
    template<typename Type>
    constexpr type_info(in_place_type_t<Type>) noexcept
        : seq_index_{type_seq_index<std::remove_cv_t<std::remove_reference_t<Type>>>::value()} {
    }

    constexpr type_seq_index_type seq_index() const noexcept {
        return seq_index_;
    }

private:
    type_seq_index_type seq_index_;
};

constexpr bool operator==(const type_info &lhs, const type_info &rhs) noexcept {
    return lhs.seq_index() == rhs.seq_index();
}

constexpr bool operator!=(const type_info &lhs, const type_info &rhs) noexcept {
    return !(lhs == rhs);
}

constexpr bool operator<(const type_info &lhs, const type_info &rhs) noexcept {
    return lhs.seq_index() < rhs.seq_index();
}

constexpr bool operator>(const type_info &lhs, const type_info &rhs) noexcept {
    return lhs.seq_index() > rhs.seq_index();
}

constexpr bool operator<=(const type_info &lhs, const type_info &rhs) noexcept {
    return !(lhs > rhs);
}

constexpr bool operator>=(const type_info &lhs, const type_info &rhs) noexcept {
    return !(lhs < rhs);
}

/**
 * @brief Returns the type info object associated to a given type.
 *
 * If type is a reference type, the result refers to a type_info object 
 * representing the referenced type (that is, type_id(T&) == type_id(T)).
 * In all cases, top-level cv-qualifiers are ignored by type_id (that is, 
 * type_id(const T) == type_id(T)).
 * These behaviors are same as native typeid operator.
 *
 * @tparam Type
 * @return const type_info&
 */
template<typename Type>
std::enable_if_t<std::is_same_v<Type, std::remove_cv_t<std::remove_reference_t<Type>>>, const type_info &>
type_id() noexcept {
    static type_info instance{in_place_type<Type>};
    return instance;
}

template<typename Type>
std::enable_if_t<!std::is_same_v<Type, std::remove_cv_t<std::remove_reference_t<Type>>>, const type_info &>
type_id() noexcept {
    return type_id<std::remove_cv_t<std::remove_reference_t<Type>>>();
}

template<typename Type>
const type_info& type_id(Type&&) noexcept {
    return type_id<std::remove_cv_t<std::remove_reference_t<Type>>>();
}

} // namespace nodec

#endif