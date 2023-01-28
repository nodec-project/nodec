#ifndef NODEC__CONTAINERS__SPARSE_VECTOR_HPP_
#define NODEC__CONTAINERS__SPARSE_VECTOR_HPP_

#include "../utility.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace nodec {
namespace containers {

// This code based on
//  * <https://github.com/sparsehash/sparsehash/blob/master/src/sparsehash/sparsetable>
// Thank you! :)

/**
 * The smaller this is, the faster lookup is (because the group bitmap is
 * smaller) and the faster insert it, because there's less to move.
 * On the other hand, there are more groups.
 */
constexpr std::uint16_t DEFAULT_SPARSE_GROUP_SIZE = 48;

template<typename Value, std::uint16_t GROUP_SIZE>
class BasicSparseGroup {
public:
    using value_type = Value;
    using size_type = std::uint16_t;

private:
    /**
     * @brief i bits to bytes (rounded down)
     */
    static size_type bit_to_byte(size_type i) {
        return i >> 3;
    }

    /**
     * @brief Gets the leftover bits with division by byte.
     */
    static size_type mod_bit(size_type i) {
        return 1 << (i & 0x07);
    }

    static size_type bits_in_byte(uint8_t byte) {
        // clang-format off
        static const uint8_t bits_in[256] = {
            0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
        };
        // clang-format on
        return bits_in[byte];
    }

    /*
     * @brief We need a small function that tells us how many set bits there are
     *   in position 0..i-1 of the bitmap.
     */
    static size_type pos_to_offset(const uint8_t *bm, size_type pos) {
        size_type retval = 0;

        // @note Condition pos > 8 is an optimization; convince yourself we
        // give exactly the same result as if we had pos >= 8 here instead.
        for (; pos > 8; pos -= 8) {
            retval += bits_in_byte(*bm++);
        }
        return retval + bits_in_byte(*bm & ((1 << pos) - 1));
    }

    /**
     * @brief Tests bucket i is occurred or not.
     */
    int bm_test(size_type i) const {
        return bitmap_[bit_to_byte(i)] & mod_bit(i);
    }

    void bm_set(size_type i) {
        bitmap_[bit_to_byte(i)] |= mod_bit(i);
    }

    void bm_clear(size_type i) {
        bitmap_[bit_to_byte(i)] &= ~mod_bit(i);
    }

    // size_type calculate_growth(size_type new_capacity) {
    //     // given _Oldcapacity and _Newsize, calculate geometric growth
    //     const size_type old_capacity = buckets_.size();

    //    if (old_capacity > GROUP_SIZE - old_capacity / 2) {
    //        return GROUP_SIZE; // geometric growth would overflow
    //    }

    //    const size_type geometric = old_capacity + old_capacity / 2;

    //    if (geometric < new_capacity) {
    //        return new_capacity; // geometric growth would be insufficient
    //    }

    //    return geometric; // geometric growth is sufficient
    //}

public:
    BasicSparseGroup() {
        buckets_.reserve(GROUP_SIZE);
    }
    ~BasicSparseGroup() {}

    bool contains(const size_type i) const {
        return bm_test(i) != 0x00;
    }

    template<typename... Args>
    std::pair<value_type &, bool> emplace(const size_type i, Args &&...args) {
        if (bm_test(i)) {
            return {buckets_[pos_to_offset(bitmap_, i)], false};
        }

        const auto offset = pos_to_offset(bitmap_, i);
        buckets_.emplace(buckets_.begin() + offset, std::forward<Args>(args)...);

        bm_set(i);

        return {buckets_[offset], true};
    }

    const value_type *try_get(const size_type i) const {
        if (!bm_test(i)) return nullptr;

        return &buckets_[pos_to_offset(bitmap_, i)];
    }

    value_type *try_get(const size_type i) {
        if (!bm_test(i)) return nullptr;

        return &buckets_[pos_to_offset(bitmap_, i)];
    }

    bool erase(const size_type i) {
        if (!bm_test(i)) return false;

        auto offset = pos_to_offset(bitmap_, i);
        buckets_.erase(buckets_.begin() + offset);
        bm_clear(i);
        return true;
    }

    /**
     * @brief Returns the number of actually existing buckets.
     */
    size_type bucket_count() const noexcept {
        return buckets_.capacity();
    }

private:
    std::vector<value_type> buckets_;
    std::uint8_t bitmap_[(GROUP_SIZE - 1) / 8 + 1]{}; // fancy math is so we round up

    // NOTE: Consider to save the next group index.
    //  To iterate the values.
    // size_type next_group_;
};

template<typename T, uint16_t GROUP_SIZE>
class BasicSparseTable {
    using Group = BasicSparseGroup<T, GROUP_SIZE>;

public:
    using size_type = std::size_t;
    using value_type = T;

public:
    BasicSparseTable() {}
    ~BasicSparseTable() {}

    // constexpr std::uint16_t group_size() const noexcept {
    //     return GROUP_SIZE;
    // }

private:
    /**
     * @brief Returns the index of the group in which the given index belongs.
     */
    size_type group_index(const size_type i) const {
        return i / GROUP_SIZE;
    }

    uint16_t pos_in_group(const size_type i) const {
        return static_cast<uint16_t>(i % GROUP_SIZE);
    }

    Group *group_assured(size_type i) {
        const auto index = group_index(i);
        if (!(index < groups_.size())) {
            groups_.resize(index + 1);
        }

        auto &group = groups_[index];
        if (group) return group.get();

        group.reset(new Group());
        return group.get();
    }

    const Group *group_if_exists(size_type i) const {
        const auto index = group_index(i);
        if (!(index < groups_.size())) return nullptr;
        auto &group = groups_[index];

        return groups_[index].get();
    }

    Group *group_if_exists(size_type i) {
        return const_cast<Group *>(as_const(*this).group_if_exists(i));
    }

public:
    value_type &operator[](const size_type i) {
        return group_assured(i)->emplace(pos_in_group(i)).first;
    }

    const value_type *try_get(size_type i) const {
        auto *group = group_if_exists(i);
        if (!group) return nullptr;

        return group->try_get(pos_in_group(i));
    }

    T *try_get(size_type i) {
        auto *group = group_if_exists(i);
        if (!group) {
            return nullptr;
        }
        return group->try_get(pos_in_group(i));
    }

    bool erase(size_type i) {
        auto *group = group_if_exists(i);
        if (!group) return false;

        return group->erase(pos_in_group(i));
    }

    bool contains(size_type i) const {
        auto *group = group_if_exists(i);
        if (!group) return false;

        return group->contains(pos_in_group(i));
    }

    ///**
    // * @brief Returns the number of actually existing buckets.
    // */
    // size_type group_bucket_count(size_type group_index) const {
    //    if (!(group_index < groups_.size())) return 0;
    //    auto &group = groups_[group_index];
    //    if (!group) return 0;
    //    return group->bucket_count();
    //}

private:
    std::vector<std::unique_ptr<Group>> groups_;
};

template<typename T>
using SparseTable = BasicSparseTable<T, DEFAULT_SPARSE_GROUP_SIZE>;

} // namespace containers
} // namespace nodec

#endif