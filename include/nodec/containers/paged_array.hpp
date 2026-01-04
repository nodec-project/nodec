#ifndef NODEC__CONTAINERS__PAGED_ARRAY_HPP_
#define NODEC__CONTAINERS__PAGED_ARRAY_HPP_

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>    

namespace nodec {
namespace containers {

namespace detail {

/**
 * @brief Compile-time log2 for power-of-2 values.
 */
template<std::size_t N, std::size_t Shift = 0>
struct log2_impl {
    static constexpr std::size_t value = log2_impl<(N >> 1), Shift + 1>::value;
};

template<std::size_t Shift>
struct log2_impl<1, Shift> {
    static constexpr std::size_t value = Shift;
};

template<std::size_t N>
inline constexpr std::size_t log2_v = log2_impl<N>::value;

/**
 * @brief Check if value is a power of 2 at compile time.
 */
template<std::size_t N>
inline constexpr bool is_power_of_2_v = (N > 0) && ((N & (N - 1)) == 0);

} // namespace detail

/**
 * @brief Default page size for PagedArray.
 *
 * 1024 elements per page.
 * Smaller page size reduces memory waste for sparse indices.
 *
 * @par RAM Usage Formula
 *
 * Total RAM = Pointer Array + Page Data
 * - Pointer array: (max_index / PageSize) × 8 bytes (fixed overhead)
 * - Page data: num_allocated_pages × PageSize × sizeof(T)
 *
 * @par Pointer Array Overhead by PageSize and Index Range
 *
 * | Index Range | Max Index       | PS=1024 | PS=2048 | PS=4096 |
 * |-------------|-----------------|---------|---------|---------|
 * | uint16      | 65,535          | 512 B   | 256 B   | 128 B   |
 * | uint20      | 1,048,575       | 8 KB    | 4 KB    | 2 KB    |
 * | uint32      | 4,294,967,295   | 32 MB   | 16 MB   | 8 MB    |
 *
 * @par Page Data Size (1 Page) by PageSize and sizeof(T)
 *
 * | sizeof(T) | PS=1024 | PS=2048 | PS=4096 |
 * |-----------|---------|---------|---------|
 * | 1 B       | 1 KB    | 2 KB    | 4 KB    |
 * | 8 B       | 8 KB    | 16 KB   | 32 KB   |
 * | 48 B      | 48 KB   | 96 KB   | 192 KB  |
 *
 * @par Total RAM Examples (sizeof(T) = 48B, uint20 range, 10 pages allocated)
 *
 * | PageSize | Pointer Array | 10 Pages | Total   |
 * |----------|---------------|----------|---------|
 * | 1024     | 8 KB          | 480 KB   | 488 KB  |
 * | 2048     | 4 KB          | 960 KB   | 964 KB  |
 * | 4096     | 2 KB          | 1.9 MB   | 1.9 MB  |
 *
 * @par Worst Case (sizeof(T) = 48B, all pages allocated)
 *
 * | Index Range | PS=1024  | PS=2048  | PS=4096  |
 * |-------------|----------|----------|----------|
 * | uint16      | 3 MB     | 3 MB     | 3 MB     |
 * | uint20      | 48 MB    | 48 MB    | 48 MB    |
 * | uint32      | 192 GB   | 192 GB   | 192 GB   |
 *
 * @par PageSize Selection Guide
 *
 * | Use Case                    | Recommended | Reason                          |
 * |-----------------------------|-------------|---------------------------------|
 * | Dense indices               | 4096        | Less page management overhead   |
 * | Sparse indices (uint20)     | 1024        | Less wasted slots per page      |
 * | Sparse indices (uint32)     | 4096        | Minimize pointer array (8 MB)   |
 * | Memory-critical             | 1024        | Finer allocation granularity    |
 *
 * @note For uint32 range, pointer array is 8-32 MB regardless of actual usage.
 *       Consider hash-based containers for very sparse uint32 indices.
 */
inline constexpr std::size_t default_page_size = 1024;

/**
 * @brief A paged sparse array with O(1) lookup.
 *
 * This container provides array-like access with lazy page allocation.
 * Pages are only allocated when accessed, making it memory-efficient
 * for sparse index usage.
 *
 * @note Null/empty detection is the caller's responsibility.
 *       The container does not track which elements are "valid".
 *       Use a sentinel value or flag in your value type (e.g., FontCharacter.initialized).
 *
 * @tparam T Value type (must be default-constructible)
 * @tparam PageSize Number of elements per page (default: 4096)
 * @tparam Allocator Allocator type for pages
 */
template<typename T, std::size_t PageSize = default_page_size,
         typename Allocator = std::allocator<T>>
class BasicPagedArray {
    static_assert(detail::is_power_of_2_v<PageSize>,
                  "PageSize must be a power of 2");
    static_assert(std::is_default_constructible<T>::value,
                  "T must be default constructible");

public:
    // --- Type aliases (STL-like) ---
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;
    using allocator_type = Allocator;

    static constexpr size_type page_size = PageSize;

private:
    using alloc_traits = std::allocator_traits<Allocator>;
    using page_pointer = pointer;

    // Compile-time constants for bit operations
    static constexpr size_type page_shift = detail::log2_v<PageSize>;
    static constexpr size_type offset_mask = PageSize - 1;

    std::vector<page_pointer> pages_;
    Allocator allocator_;
    value_type fill_value_{};

    /**
     * @brief Calculate page index from element index.
     * Uses bit shift instead of division for power-of-2 PageSize.
     */
    static constexpr size_type page_index(size_type index) noexcept {
        return index >> page_shift;
    }

    /**
     * @brief Calculate offset within page from element index.
     * Uses bit AND instead of modulo for power-of-2 PageSize.
     */
    static constexpr size_type page_offset(size_type index) noexcept {
        return index & offset_mask;
    }

    /**
     * @brief Allocate and construct a new page with fill_value_.
     */
    page_pointer allocate_page() {
        page_pointer page = alloc_traits::allocate(allocator_, PageSize);
        for (size_type i = 0; i < PageSize; ++i) {
            alloc_traits::construct(allocator_, page + i, fill_value_);
        }
        return page;
    }

    /**
     * @brief Destroy and deallocate a page.
     */
    void deallocate_page(page_pointer page) noexcept {
        if (page) {
            // Use destroy for batch destruction
            for (size_type i = 0; i < PageSize; ++i) {
                alloc_traits::destroy(allocator_, page + i);
            }
            alloc_traits::deallocate(allocator_, page, PageSize);
        }
    }

public:
    // --- Constructors / Destructor ---

    BasicPagedArray() noexcept(noexcept(Allocator()))
        : pages_{}, allocator_{}, fill_value_{} {}

    explicit BasicPagedArray(const value_type &fill_value) noexcept(noexcept(Allocator()))
        : pages_{}, allocator_{}, fill_value_{fill_value} {}

    explicit BasicPagedArray(const Allocator &alloc) noexcept
        : pages_{}, allocator_{alloc}, fill_value_{} {}

    BasicPagedArray(const value_type &fill_value, const Allocator &alloc) noexcept
        : pages_{}, allocator_{alloc}, fill_value_{fill_value} {}

    ~BasicPagedArray() {
        clear();
    }

    // Non-copyable (can be made copyable if needed)
    BasicPagedArray(const BasicPagedArray &) = delete;
    BasicPagedArray &operator=(const BasicPagedArray &) = delete;

    // Movable
    BasicPagedArray(BasicPagedArray &&other) noexcept
        : pages_{std::move(other.pages_)},
          allocator_{std::move(other.allocator_)},
          fill_value_{std::move(other.fill_value_)} {}

    BasicPagedArray &operator=(BasicPagedArray &&other) noexcept {
        if (this != &other) {
            clear();
            pages_ = std::move(other.pages_);
            allocator_ = std::move(other.allocator_);
            fill_value_ = std::move(other.fill_value_);
        }
        return *this;
    }

    // --- Element access ---

    /**
     * @brief Access element at index, allocating page if necessary.
     *
     * @param index Element index
     * @return Reference to element (default-constructed if new)
     */
    reference operator[](size_type index) {
        return *assure(index);
    }

    /**
     * @brief Try to get pointer to element without allocating.
     *
     * @param index Element index
     * @return Pointer to element, or nullptr if page not allocated
     */
    pointer try_get(size_type index) noexcept {
        const auto page_idx = page_index(index);
        if (page_idx >= pages_.size() || !pages_[page_idx]) {
            return nullptr;
        }
        return pages_[page_idx] + page_offset(index);
    }

    const_pointer try_get(size_type index) const noexcept {
        const auto page_idx = page_index(index);
        if (page_idx >= pages_.size() || !pages_[page_idx]) {
            return nullptr;
        }
        return pages_[page_idx] + page_offset(index);
    }

    /**
     * @brief Ensure element exists at index, allocating page if necessary.
     *
     * @param index Element index
     * @return Pointer to element
     */
    pointer assure(size_type index) {
        const auto page_idx = page_index(index);

        if (page_idx >= pages_.size()) {
            pages_.resize(page_idx + 1, nullptr);
        }

        if (!pages_[page_idx]) {
            pages_[page_idx] = allocate_page();
        }

        return pages_[page_idx] + page_offset(index);
    }

    // --- Page management ---

    /**
     * @brief Check if page containing index is allocated.
     */
    bool has_page(size_type index) const noexcept {
        const auto page_idx = page_index(index);
        return page_idx < pages_.size() && pages_[page_idx] != nullptr;
    }

    /**
     * @brief Get number of allocated pages.
     */
    size_type allocated_page_count() const noexcept {
        size_type count = 0;
        for (const auto &page : pages_) {
            if (page) ++count;
        }
        return count;
    }

    /**
     * @brief Get total capacity (pages_.size() * PageSize).
     */
    size_type capacity() const noexcept {
        return pages_.size() * PageSize;
    }

    /**
     * @brief Get memory usage in bytes (approximate).
     */
    size_type memory_usage() const noexcept {
        return pages_.size() * sizeof(page_pointer) +
               allocated_page_count() * PageSize * sizeof(T);
    }

    /**
     * @brief Clear all pages and deallocate memory.
     */
    void clear() {
        for (auto &page : pages_) {
            deallocate_page(page);
            page = nullptr;
        }
        pages_.clear();
    }

    /**
     * @brief Shrink pages_ vector to fit allocated pages.
     *
     * Removes trailing null pages from the pages_ vector.
     */
    void shrink_to_fit() {
        while (!pages_.empty() && !pages_.back()) {
            pages_.pop_back();
        }
        pages_.shrink_to_fit();
    }

    // --- Allocator access ---

    allocator_type get_allocator() const noexcept {
        return allocator_;
    }

    // --- Iterator ---

    /**
     * @brief Iterator over all elements in allocated pages.
     *
     * Iterates through all slots in allocated pages (including uninitialized ones).
     * Use value's null-check mechanism to filter valid elements.
     */
    template<bool Const>
    class basic_iterator {
        friend class BasicPagedArray;

        using container_type = std::conditional_t<Const, const BasicPagedArray, BasicPagedArray>;
        using page_vector = std::conditional_t<Const,
                                                const std::vector<page_pointer>,
                                                std::vector<page_pointer>>;

        container_type *container_;
        size_type page_idx_;
        size_type offset_;

        basic_iterator(container_type *container, size_type page_idx, size_type offset)
            : container_{container}, page_idx_{page_idx}, offset_{offset} {
            skip_empty_pages();
        }

        void skip_empty_pages() {
            while (page_idx_ < container_->pages_.size() &&
                   !container_->pages_[page_idx_]) {
                ++page_idx_;
                offset_ = 0;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<Const, const T *, T *>;
        using reference = std::conditional_t<Const, const T &, T &>;

        basic_iterator() : container_{nullptr}, page_idx_{0}, offset_{0} {}

        reference operator*() const {
            return container_->pages_[page_idx_][offset_];
        }

        pointer operator->() const {
            return container_->pages_[page_idx_] + offset_;
        }

        /**
         * @brief Get the logical index of current element.
         * Uses bit shift instead of multiplication.
         */
        size_type index() const noexcept {
            return (page_idx_ << page_shift) + offset_;
        }

        basic_iterator &operator++() {
            ++offset_;
            if (offset_ >= PageSize) {
                offset_ = 0;
                ++page_idx_;
                skip_empty_pages();
            }
            return *this;
        }

        basic_iterator operator++(int) {
            basic_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const basic_iterator &other) const noexcept {
            return container_ == other.container_ &&
                   page_idx_ == other.page_idx_ &&
                   offset_ == other.offset_;
        }

        bool operator!=(const basic_iterator &other) const noexcept {
            return !(*this == other);
        }
    };

    using iterator = basic_iterator<false>;
    using const_iterator = basic_iterator<true>;

    /**
     * @brief Begin iterator (first element in first allocated page).
     */
    iterator begin() noexcept {
        return iterator{this, 0, 0};
    }

    const_iterator begin() const noexcept {
        return const_iterator{this, 0, 0};
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    /**
     * @brief End iterator.
     */
    iterator end() noexcept {
        return iterator{this, pages_.size(), 0};
    }

    const_iterator end() const noexcept {
        return const_iterator{this, pages_.size(), 0};
    }

    const_iterator cend() const noexcept {
        return end();
    }
};

/**
 * @brief Default PagedArray with page size 4096.
 */
template<typename T>
using PagedArray = BasicPagedArray<T, default_page_size>;

} // namespace containers
} // namespace nodec

#endif
