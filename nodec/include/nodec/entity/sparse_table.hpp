#ifndef NODEC__ENTITY__SPARSE_TABLE_HPP_
#define NODEC__ENTITY__SPARSE_TABLE_HPP_

#include <nodec/logging.hpp>

#include <vector>


namespace nodec
{
namespace entity
{


/**
* The smaller this is, the faster lookup is (because the group bitmap is
* smaller) and the faster insert it, because there's less to move.
* On the other hand, there are more groups.
*/
constexpr uint16_t DEFAULT_SPARSE_GROUP_SIZE = 48;

template<typename T, uint16_t GROUP_SIZE>
class BasicSparseGroup
{

public:
    using size_type = uint16_t;

private:

    /**
    * @brief i bits to bytes (rounded down)
    */
    static size_type bytebit(size_type i) { return i >> 3; }

    /**
    * @brief Gets the leftover bits with division by byte.
    */
    static size_type modbit(size_type i) { return 1 << (i & 0x07); }

    /**
    * @brief Tests bucket i is occuoued or not.
    */
    int bmtest(size_type i) const { return bitmap[bytebit(i)] & modbit(i); }

    void bmset(size_type i) { bitmap[bytebit(i)] |= modbit(i); }

    void bmclear(size_type i) { bitmap[bytebit(i)] &= ~modbit(i); }

    static size_type bits_in_byte(uint8_t byte)
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
    static size_type pos_to_offset(const uint8_t* bm, size_type pos)
    {
        size_type retval = 0;

        // @note Condition pos > 8 is an optimization; convince yourself we
        // give exactly the same result as if we had pos >= 8 here instead.
        for (; pos > 8; pos -= 8)
        {
            retval += bits_in_byte(*bm++);
        }
        return retval + bits_in_byte(*bm & ((1 << pos) - 1));
    }

public:
    size_type num_buckets() { return num_buckets_; }

    T& operator[](size_type i)
    {
        if (bmtest(i))
        {
            return group[pos_to_offset(bitmap, i)];
        }

        auto offset = pos_to_offset(bitmap, i);
        group.insert(group.begin() + offset, T());
        ++num_buckets_;
        bmset(i);
        return group[offset];
    }

    void erase(size_type i)
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

    void test()
    {
        nodec::logging::InfoStream info_stream(__FILE__, __LINE__);
        for (auto& ele : group)
        {
             info_stream<< ele << ", ";
        }

        for (auto& byte : bitmap)
        {
            info_stream << static_cast<int>(byte) << ", ";
        }

    }


private:
    std::vector<T> group;
    uint8_t bitmap[(GROUP_SIZE - 1) / 8 + 1]; // fancy math is so we round up
    size_type num_buckets_; // limits GROUP_SIZE to 64KB

};


template<typename T, uint16_t GROUP_SIZE>
class BasicSparseTable
{
public:
    using Group = BasicSparseGroup<T, GROUP_SIZE>;

public:

    uint16_t pos_in_group(size_t i) const
    {
        return static_cast<uint16_t>(i % GROUP_SIZE);
    }

    size_t group_num(size_t i) const
    {
        return i / GROUP_SIZE;
    }

    Group& which_group(size_t i)
    {
        auto num = group_num(i);

        if (!(num < groups.size()))
        {
            groups.resize(num + 1);
        }

        return groups[num];
    }


    T& operator[](size_t i)
    {
        return which_group(i)[pos_in_group(i)];
    }

    void erase(size_t i)
    {
        which_group(i).erase(pos_in_group(i));
    }


    void test()
    {
        for (auto& group : groups)
        {
            group.test();
        }
    }


private:
    std::vector<Group> groups;
};


template<typename T>
using SparseTable = BasicSparseTable<T, DEFAULT_SPARSE_GROUP_SIZE>;


}
}

#endif