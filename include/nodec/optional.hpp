#ifndef NODEC__OPTIONAL_HPP_
#define NODEC__OPTIONAL_HPP_

#include "type_traits.hpp"
#include "utility.hpp"

#include <stdexcept>
#include <type_traits>
#include <utility>

// This class is trying to be implemented to work with C++11 as well.
// class naming rule follows std::optional for compatibility.

// This code is based on the followings.
//  * https://github.com/TartanLlama/optional

namespace nodec {

template<class T>
class optional;

template<class T>
class optional;
namespace internal {

template<class T, bool = std::is_trivially_destructible<T>::value>
struct optional_storage_base {
    // trivially destructive.

    constexpr optional_storage_base() noexcept
        : dummy_{}, has_value{false} {}

    template<class... Args>
    constexpr explicit optional_storage_base(in_place_t, Args &&...args)
        // NOTE: Not use '{}' initializer, or the compiler consider aggregate initialization!
        : value(std::forward<Args>(args)...),
          has_value{true} {}

    // No destructor since this class is trivially destructive

    optional_storage_base(const optional_storage_base &other)
        : value{other.value},
          has_value{other.has_value} {}

    optional_storage_base(optional_storage_base &&other) noexcept
        : value{std::move(other.value)},
          has_value{std::move(other.has_value)} {}

    optional_storage_base &operator=(const optional_storage_base &other) {
        value = other.value;
        has_value = other.has_value;
        return *this;
    }

    optional_storage_base &operator=(optional_storage_base &&other) noexcept {
        value = std::move(other.value);
        has_value = std::move(other.has_value);
        return *this;
    }

    void reset() noexcept {
        has_value = false;
    }

    struct Dummy {
        // This default constructor is user-provided to avoid zero-initialization when objects are value-initialized.
        constexpr Dummy() noexcept {}
    };

    union {
        Dummy dummy_;
        typename std::remove_const<T>::type value;
    };

    bool has_value;
};

template<class T>
struct optional_storage_base<T, false> {
    // not trivially destructible.

    constexpr optional_storage_base() noexcept
        : dummy_{}, has_value{false} {}

    template<class... Args>
    constexpr explicit optional_storage_base(in_place_t, Args &&...args)
        : value(std::forward<Args>(args)...), has_value{true} {}

    ~optional_storage_base() noexcept {
        if (has_value) {
            value.~T();
        }
    }

    optional_storage_base(const optional_storage_base &other)
        : value{other.value},
          has_value{other.has_value} {}

    optional_storage_base(optional_storage_base &&other) noexcept
        : value{std::move(other.value)},
          has_value{std::move(other.has_value)} {}

    optional_storage_base &operator=(const optional_storage_base &other) {
        value = other.value;
        has_value = other.has_value;
        return *this;
    }

    optional_storage_base &operator=(optional_storage_base &&other) noexcept {
        value = std::move(other.value);
        has_value = std::move(other.has_value);
        return *this;
    }

    constexpr void reset() noexcept {
        if (has_value) {
            value.~T();
            has_value = false;
        }
    }

    struct Dummy {
        // This default constructor is user-provided to avoid zero-initialization when objects are value-initialized.
        constexpr Dummy() noexcept {}
    };
    union {
        Dummy dummy_;
        typename std::remove_const<T>::type value;
    };

    bool has_value;
};

template<class T>
struct optional_storage : optional_storage_base<T> {
    using optional_storage_base<T>::optional_storage_base;

    template<class... Args>
    constexpr T &construct(Args &&...args) {
        new (std::addressof(this->value)) T(std::forward<Args>(args)...);
        this->has_value = true;
        return this->value;
    }

    template<class U>
    constexpr void assign(U &&right) {
        if (this->has_value) {
            this->value = std::forward<U>(right);
        } else {
            construct(std::forward<U>(right));
        }
    }
};

template<class T, class U>
using enable_forward_value =
    std::enable_if<std::is_constructible<T, U &&>::value
                   && !std::is_same<typename std::decay<U>::type, in_place_t>::value
                   && !std::is_same<typename std::decay<U>::type, optional<T>>::value>;

template<class T, class U, class Other>
using enable_from_other =
    std::enable_if<
        std::is_constructible<T, Other>::value
        && !std::is_constructible<T, optional<U> &>::value
        && !std::is_constructible<T, optional<U> &&>::value
        && !std::is_constructible<T, const optional<U> &>::value
        && !std::is_constructible<T, const optional<U> &&>::value
        && !std::is_convertible<optional<U> &, T>::value
        && !std::is_convertible<optional<U> &&, T>::value
        && !std::is_convertible<const optional<U> &, T>::value
        && !std::is_convertible<const optional<U> &&, T>::value>;

template<class T, class U>
using enable_assign_forward =
    std::enable_if<
        !std::is_same<optional<T>, typename std::decay<U>::type>::value
        && !(std::is_scalar<T>::value && std::is_same<T, typename std::decay<U>::type>::value)
        && std::is_constructible<T, U>::value && std::is_assignable<T &, U>::value>;

template<class T, class U, class Other>
using enable_assign_from_other = std::enable_if<
    std::is_constructible<T, Other>::value
    && std::is_assignable<T &, Other>::value
    && !std::is_constructible<T, optional<U> &>::value
    && !std::is_constructible<T, optional<U> &&>::value
    && !std::is_constructible<T, const optional<U> &>::value
    && !std::is_constructible<T, const optional<U> &&>::value
    && !std::is_convertible<optional<U> &, T>::value
    && !std::is_convertible<optional<U> &&, T>::value
    && !std::is_convertible<const optional<U> &, T>::value
    && !std::is_convertible<const optional<U> &&, T>::value
    && !std::is_assignable<T &, optional<U> &>::value
    && !std::is_assignable<T &, optional<U> &&>::value
    && !std::is_assignable<T &, const optional<U> &>::value
    && !std::is_assignable<T &, const optional<U> &&>::value>;

} // namespace internal

struct nullopt_t {};

constexpr nullopt_t nullopt;

class bad_optional_access : public std::exception {
public:
    bad_optional_access() = default;
    const char *what() const noexcept {
        return "Bad optional access";
    }
};

template<class T>
class optional {
    static_assert(!std::is_same<T, in_place_t>::value,
                  "instantiation of optional with in_place_t is ill-formed");
    static_assert(!std::is_same<typename std::decay<T>::type, nullopt_t>::value,
                  "instantiation of optional with nullopt_t is ill-formed");

public:
    using value_type = T;

    constexpr optional() noexcept = default;
    constexpr optional(nullopt_t) noexcept {}

    constexpr optional(const optional &) = default;
    constexpr optional(optional &&) = default;

    template<class... Args, typename std::enable_if<std::is_constructible<T, Args...>::value>::type * = nullptr>
    constexpr explicit optional(in_place_t, Args &&...args)
        : storage_{in_place, std::forward<Args>(args)...} {}

    template<class Elem, class... Args, typename std::enable_if<std::is_constructible<T, std::initializer_list<Elem> &, Args...>::value>::type * = nullptr>
    constexpr explicit optional(in_place_t, std::initializer_list<Elem> list, Args &&...args)
        : storage_{in_place, list, std::forward<Args>(args)...} {}

    template<class U = T,
             typename std::enable_if<std::is_convertible<U &&, T>::value>::type * = nullptr,
             typename internal::enable_forward_value<T, U>::type * = nullptr>
    constexpr optional(U &&u)
        : storage_{in_place, std::forward<U>(u)} {}

    template<class U = T,
             typename std::enable_if<!std::is_convertible<U &&, T>::value>::type * = nullptr,
             typename internal::enable_forward_value<T, U>::type * = nullptr>
    constexpr explicit optional(U &&u)
        : storage_{in_place, std::forward<U>(u)} {}

    template<class U,
             typename internal::enable_from_other<T, U, const U &>::type * = nullptr,
             typename std::enable_if<std::is_convertible<const U &, T>::value>::type * = nullptr>
    optional(const optional<U> &right) {
        if (right.has_value()) {
            storage_.construct(*right);
        }
    }

    template<class U,
             typename internal::enable_from_other<T, U, const U &>::type * = nullptr,
             typename std::enable_if<!std::is_convertible<const U &, T>::value>::type * = nullptr>
    explicit optional(const optional<U> &right) {
        if (right.has_value()) {
            storage_.construct(*right);
        }
    }

    ~optional() = default;

    constexpr optional &operator=(nullopt_t) noexcept {
        storage_.reset();
        return *this;
    }

    optional &operator=(const optional &) = default;
    optional &operator=(optional &&) = default;

    template<class U = T,
             typename internal::enable_assign_forward<T, U>::type * = nullptr>
    constexpr optional &operator=(U &&right) {
        storage_.assign(std::forward<U>(right));
        return *this;
    }

    template<class U,
             typename internal::enable_assign_from_other<T, U, const U &>::type * = nullptr>
    constexpr optional &operator=(const optional<U> &right) {
        if (right) {
            storage_.assign(*right);
        } else {
            storage_.reset();
        }
        return *this;
    }

    template<class U,
             typename internal::enable_assign_from_other<T, U, U>::type * = nullptr>
    constexpr optional &operator=(optional<U> &&right) {
        if (right) {
            storage_.assign(std::move(*right));
        } else {
            storage_.reset();
        }
        return *this;
    }

    template<class... Args>
    constexpr T &emplace(Args &&...args) {
        storage_.reset();
        return storage_.construct(std::forward<Args>(args)...);
    }

    template<class Elem, class... Args,
             typename std::enable_if<std::is_constructible<T, std::initializer_list<Elem> &, Args...>::value>::type * = nullptr>
    constexpr T &emplace(std::initializer_list<Elem> list, Args &&...args) {
        storage_.reset();
        return storage_.construct(list, std::forward<Args>(args)...);
    }

    constexpr explicit
    operator bool() const noexcept {
        return storage_.has_value;
    }

    constexpr bool has_value() const noexcept {
        return storage_.has_value;
    }

    /**
     * @brief Returns the contained value if there is one, otherwise throws an exception.
     *
     * @return constexpr const T&
     */
    constexpr const T &value() const & {
        if (!has_value()) throw bad_optional_access();
        return storage_.value;
    }

    constexpr T &value() & {
        if (!has_value()) throw bad_optional_access();
        return storage_.value;
    }

    constexpr const T &&value() const && {
        if (!has_value()) throw bad_optional_access();
        return std::move(storage_.value);
    }

    constexpr T &&value() && {
        if (!has_value()) throw bad_optional_access();
        return std::move(storage_.value);
    }

    /**
     * @brief Returns the contained value if there is one,
     * otherwise it results in undefined behavior.
     * 
     * Ensure that the optional has a value (has_value() == true) 
     * before calling this function.
     *
     * @return constexpr T&
     */
    constexpr T &operator*() & {
        return storage_.value;
    }

    constexpr const T &operator*() const & {
        return storage_.value;
    }

    constexpr const T &&operator*() const && {
        return std::move(storage_.value);
    }

    constexpr T &&operator*() && {
        return std::move(storage_.value);
    }

    /**
     * @brief Returns a pointer to the contained value if there is one,
     * otherwise it results in undefined behavior.
     *
     * Ensure that the optional has a value (has_value() == true)
     * before calling this function.
     * 
     * @return constexpr const T*
     */
    constexpr const T *operator->() const noexcept {
        return std::addressof(storage_.value);
    }

    constexpr T *operator->() noexcept {
        return std::addressof(storage_.value);
    }

    constexpr void reset() noexcept {
        storage_.reset();
    }

private:
    internal::optional_storage<T> storage_;
};

} // namespace nodec

#endif