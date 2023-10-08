#ifndef NODEC__ARRAY_VIEW_HPP_
#define NODEC__ARRAY_VIEW_HPP_

#include <iterator>
#include <type_traits>

#include "type_traits.hpp"

namespace nodec {

namespace internal {

template<class Container, typename = void>
struct enable_array_view_for : std::false_type {};

template<class Container>
struct enable_array_view_for<
    Container,
    void_t<
        decltype(std::declval<Container>().data()),
        decltype(std::declval<Container>().size())>> : std::true_type {};

} // namespace internal

/**
 * @brief The ArrayView class provides the functionalities to reference all or
 * part of a sequence of memory contiguous containers like native array,
 * std::array, and std::vector).
 *
 * @tparam ElementType
 */
template<typename ElementType>
class ArrayView {
public:
    using element_type = ElementType;
    using value_type = std::remove_cv_t<ElementType>;
    using reference = ElementType &;
    using size_type = std::size_t;
    using pointer = ElementType *;
    using iterator = pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;

    ArrayView() noexcept
        : size_(0), data_(nullptr) {}

    template<std::size_t Size>
    ArrayView(ElementType (&array)[Size]) noexcept
        : data_(array), size_(Size) {}

    ArrayView(pointer *first, std::size_t size) noexcept
        : data_(first), size_(size) {}

    template<std::size_t Size>
    ArrayView(std::array<element_type, Size> &array) noexcept
        : data_(array.data()), size_(Size) {
    }

    template<std::size_t Size>
    ArrayView(const std::array<element_type, Size> &array) noexcept
        : data_(array.data()), size_(Size) {}

    template<class Container,
             std::enable_if_t<internal::enable_array_view_for<Container>::value> * = nullptr>
    ArrayView(Container &container) noexcept
        : data_(container.data()), size_(container.size()) {
    }

    size_type size() const noexcept {
        return size_;
    }

    pointer data() const noexcept {
        return data_;
    }

    reference operator[](size_type i) const {
        return data_[i];
    }

    iterator begin() const noexcept {
        return data_;
    }

    iterator end() const noexcept {
        return data_ + size_;
    }

    reverse_iterator rbegin() const noexcept {
        return reverse_iterator(end());
    }

    reverse_iterator rend() const noexcept {
        return reverse_iterator(begin());
    }

private:
    pointer data_;
    size_type size_;
};
} // namespace nodec

#endif