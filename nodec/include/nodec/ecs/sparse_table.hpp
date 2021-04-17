#ifndef NODEC__ECS__SPARSE_TABLE_HPP_
#define NODEC__ECS__SPARSE_TABLE_HPP_

#include <nodec/logging.hpp>

#include <vector>


namespace nodec
{
namespace ecs
{


/**
* The smaller this is, the faster lookup is (because the group bitmap is
* smaller) and the faster insert it, because there's less to move.
* On the other hand, there are more groups.
*/
constexpr uint16_t DEFAULT_SPARSE_GROUP_SIZE = 48;

template<typename Table>
class TableIterator
{
public:
    using GroupIterator = typename Table::Group::Iterator;
    using Value = typename Table::Group::Value;

public:
    TableIterator(Table* table, size_t group_num, GroupIterator group_iter)
        : table_(table), group_num_(group_num), group_iter_(group_iter)
    {
    }

    TableIterator(const TableIterator& from)
        : table_(from.table_), group_num_(from.group_num_), group_iter_(from.group_iter_)
    {
    }

    Value& operator*() const
    {
        return *group_iter_;
    }

    Value* operator->() const
    {
        return group_iter_.operator->();
    }

    TableIterator& operator++()
    {
        ++group_iter_;
        if (group_iter_ == table_->end(group_num_)
            && group_num_ + 1 < table_->group_count())
        {
            ++group_num_;
            group_iter_ = table_->begin(group_num_);
        }
        return *this;
    }

    TableIterator& operator--()
    {
        if (group_iter_ == table_->begin(group_num_)
            && group_num_ > 0)
        {
            --group_num_;
            group_iter_ = table_->end(group_num_);
        }
        --group_iter_;
        return *this;
    }

    bool operator==(const TableIterator& it)
    {
        return table_ == it.table_
            && group_num_ == it.group_num_
            && group_iter_ == it.group_iter_;
    }
    bool operator!=(const TableIterator& it)
    {
        return !(*this == it);
    }


private:
    Table* table_;
    size_t group_num_;
    GroupIterator group_iter_;

};


template<typename Value, uint16_t GROUP_SIZE>
class BasicSparseGroup
{
public:
    using SizeT = uint16_t;
    using Iterator = typename std::vector<Value>::iterator;

private:
    /**
    * @brief i bits to bytes (rounded down)
    */
    static SizeT bytebit(SizeT i) { return i >> 3; }

    /**
    * @brief Gets the leftover bits with division by byte.
    */
    static SizeT modbit(SizeT i) { return 1 << (i & 0x07); }

    /**
    * @brief Tests bucket i is occuoued or not.
    */
    int bmtest(SizeT i) const { return bitmap[bytebit(i)] & modbit(i); }

    void bmset(SizeT i) { bitmap[bytebit(i)] |= modbit(i); }

    void bmclear(SizeT i) { bitmap[bytebit(i)] &= ~modbit(i); }

    static SizeT bits_in_byte(uint8_t byte)
    {
        static const uint8_t bits_in[256]{
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
        return bits_in[byte];
    }

public:
    /*
    * @brief We need a small function that tells us how many set bits there are
    *   in position 0..i-1 of the bitmap.
    */
    static SizeT pos_to_offset(const uint8_t* bm, SizeT pos)
    {
        SizeT retval = 0;

        // @note Condition pos > 8 is an optimization; convince yourself we
        // give exactly the same result as if we had pos >= 8 here instead.
        for (; pos > 8; pos -= 8)
        {
            retval += bits_in_byte(*bm++);
        }
        return retval + bits_in_byte(*bm & ((1 << pos) - 1));
    }

public:
    Iterator begin() { return group.begin(); }
    Iterator end() { return group.end(); }

    SizeT num_buckets() { return num_buckets_; }


    bool contains(const SizeT i) const
    {
        return bmtest(i) != 0x00;
    }

    template<typename... Args>
    std::pair<Value&, bool> emplace(const SizeT i, Args&&... args)
    {
        if (bmtest(i))
        {
            return { group[pos_to_offset(bitmap, i)], false };
        }

        auto offset = pos_to_offset(bitmap, i);
        group.emplace(group.begin() + offset, std::forward<Args>(args)...);
        ++num_buckets_;
        bmset(i);
        return { group[offset], true };
    }


    void erase(const SizeT i)
    {
        if (!bmtest(i))
        {
            return;
        }

        auto offset = pos_to_offset(bitmap, i);
        group.erase(group.begin() + offset);
        --num_buckets_;
        bmclear(i);
    }


private:
    std::vector<Value> group;
    uint8_t bitmap[(GROUP_SIZE - 1) / 8 + 1]; // fancy math is so we round up
    SizeT num_buckets_; // limits GROUP_SIZE to 64KB

};



template<typename T, uint16_t GROUP_SIZE>
class BasicSparseTable
{
public:
    using Group = BasicSparseGroup<T, GROUP_SIZE>;
    using SizeT = size_t;
    using Iterator = TableIterator<BasicSparseTable<T, GROUP_SIZE>>;
    using LocalIterator = typename Group::Iterator;


public:

    uint16_t pos_in_group(const SizeT i) const
    {
        return static_cast<uint16_t>(i % GROUP_SIZE);
    }

    SizeT group_num(const SizeT i) const
    {
        return i / GROUP_SIZE;
    }

    Group& which_group(SizeT i)
    {
        auto num = group_num(i);

        if (!(num < groups.size()))
        {
            groups.resize(num + 1);
        }

        return groups[num];
    }

    bool has_group(SizeT i) const
    {
        return group_num(i) < groups.size();
    }


    Iterator begin()
    {
        return Iterator(this, 0, begin(0));
    }
    Iterator end()
    {
        auto group_num = groups.size() - 1;
        return Iterator(this, group_num, end(group_num));
    }


    T& operator[](const SizeT i)
    {
        return which_group(i).emplace(pos_in_group(i)).first;
    }


    void erase(const SizeT i)
    {
        if (!has_group(i))
        {
            return;
        }

        which_group(i).erase(pos_in_group(i));
    }

    bool contains(const SizeT i)
    {
        if (!has_group(i))
        {
            return false;
        }

        return which_group(i).contains(pos_in_group(i));
    }

    SizeT group_count() const noexcept { return groups.size(); }

    LocalIterator begin(SizeT n)
    {
        return groups[n].begin();
    }
    LocalIterator end(SizeT n)
    {
        return groups[n].end();
    }


private:
    std::vector<Group> groups;
};


template<typename T>
using SparseTable = BasicSparseTable<T, DEFAULT_SPARSE_GROUP_SIZE>;


}
}

#endif