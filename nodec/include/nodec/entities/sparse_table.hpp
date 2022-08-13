#ifndef NODEC__ENTITIES__SPARSE_TABLE_HPP_
#define NODEC__ENTITIES__SPARSE_TABLE_HPP_

#include <nodec/logging.hpp>

#include <vector>

namespace nodec {
namespace entities {

// This code based on
//  * <https://github.com/sparsehash/sparsehash/blob/master/src/sparsehash/sparsetable>
// Thank you! :)

/**
 * The smaller this is, the faster lookup is (because the group bitmap is
 * smaller) and the faster insert it, because there's less to move.
 * On the other hand, there are more groups.
 */
constexpr uint16_t DEFAULT_SPARSE_GROUP_SIZE = 48;

template<typename Table>
class TableIterator {
    using GroupIterator = typename Table::Group::iterator;

public:
    using value_type = typename Table::Group::Value;
    using pointer = value_type *;
    using reference = value_type &;

public:
    TableIterator(Table *table, size_t group_num, GroupIterator group_iter)
        : table_(table), group_num_(group_num), group_iter_(group_iter) {
    }

    TableIterator(const TableIterator &from)
        : table_(from.table_), group_num_(from.group_num_), group_iter_(from.group_iter_) {
    }

    reference operator*() const {
        return *group_iter_;
    }

    pointer operator->() const {
        return group_iter_.operator->();
    }

    TableIterator &operator++() {
        ++group_iter_;
        if (group_iter_ == table_->end(group_num_)
            && group_num_ + 1 < table_->group_count()) {
            ++group_num_;
            group_iter_ = table_->begin(group_num_);
        }
        return *this;
    }

    TableIterator &operator--() {
        if (group_iter_ == table_->begin(group_num_)
            && group_num_ > 0) {
            --group_num_;
            group_iter_ = table_->end(group_num_);
        }
        --group_iter_;
        return *this;
    }

    bool operator==(const TableIterator &it) {
        return table_ == it.table_
               && group_num_ == it.group_num_
               && group_iter_ == it.group_iter_;
    }
    bool operator!=(const TableIterator &it) {
        return !(*this == it);
    }

private:
    Table *table_;
    size_t group_num_;
    GroupIterator group_iter_;
};

template<typename ValueT, uint16_t GROUP_SIZE>
class BasicSparseGroup {
public:
    using Value = ValueT;

public:
    using size_type = uint16_t;
    using iterator = typename std::vector<Value>::iterator;

private:
    /**
     * @brief i bits to bytes (rounded down)
     */
    static size_type bytebit(size_type i) {
        return i >> 3;
    }

    /**
     * @brief Gets the leftover bits with division by byte.
     */
    static size_type modbit(size_type i) {
        return 1 << (i & 0x07);
    }

    /**
     * @brief Tests bucket i is occuoued or not.
     */
    int bmtest(size_type i) const {
        return bitmap[bytebit(i)] & modbit(i);
    }

    void bmset(size_type i) {
        bitmap[bytebit(i)] |= modbit(i);
    }

    void bmclear(size_type i) {
        bitmap[bytebit(i)] &= ~modbit(i);
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

public:
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

public:
    iterator begin() {
        return group.begin();
    }
    iterator end() {
        return group.end();
    }

    size_type num_buckets() {
        return num_buckets_;
    }

    bool contains(const size_type i) const {
        return bmtest(i) != 0x00;
    }

    template<typename... Args>
    std::pair<Value &, bool> emplace(const size_type i, Args &&...args) {
        if (bmtest(i)) {
            return {group[pos_to_offset(bitmap, i)], false};
        }

        auto offset = pos_to_offset(bitmap, i);
        group.emplace(group.begin() + offset, std::forward<Args>(args)...);
        ++num_buckets_;
        bmset(i);
        return {group[offset], true};
    }

    const Value *try_get(const size_type i) const {
        if (!bmtest(i)) {
            return nullptr;
        }

        return &group[pos_to_offset(bitmap, i)];
    }

    Value *try_get(const size_type i) {
        if (!bmtest(i)) {
            return nullptr;
        }

        return &group[pos_to_offset(bitmap, i)];
    }

    bool erase(const size_type i) {
        if (!bmtest(i)) {
            return false;
        }

        auto offset = pos_to_offset(bitmap, i);
        group.erase(group.begin() + offset);
        --num_buckets_;
        bmclear(i);
        return true;
    }

private:
    std::vector<Value> group;
    uint8_t bitmap[(GROUP_SIZE - 1) / 8 + 1]; // fancy math is so we round up
    size_type num_buckets_;                   // limits GROUP_SIZE to 64KB
};

template<typename T, uint16_t GROUP_SIZE>
class BasicSparseTable {
public:
    using Group = BasicSparseGroup<T, GROUP_SIZE>;

public:
    using size_type = size_t;
    using iterator = TableIterator<BasicSparseTable<T, GROUP_SIZE>>;
    using local_iterator = typename Group::iterator;

public:
    // TODO: Support move semantics.

    uint16_t pos_in_group(const size_type i) const {
        return static_cast<uint16_t>(i % GROUP_SIZE);
    }

    size_type group_num(const size_type i) const {
        return i / GROUP_SIZE;
    }

    Group *group_assured(size_type i) {
        auto num = group_num(i);
        if (!(num < groups.size())) {
            groups.resize(num + 1);
        }
        // nodec:logging::DebugStream(__FILE__, __LINE__) << sizeof(Group) << " * " << groups.size();
        return &groups[num];
    }

    const Group *group_if_exists(size_type i) const {
        auto num = group_num(i);
        if (!(num < groups.size())) {
            return nullptr;
        }
        return &groups[num];
    }

    Group *group_if_exists(size_type i) {
        auto num = group_num(i);
        if (!(num < groups.size())) {
            return nullptr;
        }
        return &groups[num];
    }

    bool has_group(size_type i) const {
        return group_num(i) < groups.size();
    }

    iterator begin() {
        return {this, 0, begin(0)};
    }

    iterator end() {
        auto group_num = groups.size() - 1;
        return {this, group_num, end(group_num)};
    }

    T &operator[](const size_type i) {
        return group_assured(i)->emplace(pos_in_group(i)).first;
    }

    const T *try_get(const size_type i) const {
        auto *group = group_if_exists(i);
        if (!group) {
            return nullptr;
        }
        return group->try_get(pos_in_group(i));
    }

    T *try_get(const size_type i) {
        auto *group = group_if_exists(i);
        if (!group) {
            return nullptr;
        }
        return group->try_get(pos_in_group(i));
    }

    bool erase(const size_type i) {
        auto *group = group_if_exists(i);
        if (!group) {
            return false;
        }

        return group->erase(pos_in_group(i));
    }

    bool contains(const size_type i) const {
        auto *group = group_if_exists(i);
        if (!group) {
            return false;
        }

        return group->contains(pos_in_group(i));
    }

    size_type group_count() const noexcept {
        return groups.size();
    }

    local_iterator begin(size_type n) {
        return groups[n].begin();
    }
    local_iterator end(size_type n) {
        return groups[n].end();
    }

private:
    std::vector<Group> groups;
};

template<typename T>
using SparseTable = BasicSparseTable<T, DEFAULT_SPARSE_GROUP_SIZE>;

} // namespace entities
} // namespace nodec

#endif