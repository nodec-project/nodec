#ifndef NODEC__ENTITY__SPARSE_TABLE_HPP_
#define NODEC__ENTITY__SPARSE_TABLE_HPP_

#include <vector>


namespace nodec
{
namespace entity
{



template<typename T, uint16_t GROUP_SIZE>
class BasicSparseGroup
{
public:
    static constexpr uint16_t DEFAULT_GROUP_SIZE = 48;

private:

    /**
    * @brief i bits to bytes (rounded down) 
    */
    static size_t bytebit(size_t i) { return i >> 3; }

    /**
    * @brief Gets the leftover bits with division by byte.
    */
    static size_t modbit(size_t i) { return 1 << (i & 0x07); }

    int bmtest(size_t i) const { return bitmap[bytebit(i)] & modbit(i); }

    void bmset(size_t i) { bitmap[bytebit(i)] |= modbit(i); }

    void bmclear(size_t i) { bitmap[bytebit(i)] &= ~modbit(i); }

    static size_t bits_in_byte(uint8_t byte)
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
    static size_t pos_to_offset(const uint8_t* bm, size_t pos)
    {
        size_t retval = 0;

        // @note Condition pos > 8 is an optimization; convince yourself we
        // give exactly the same result as if we had pos >= 8 here instead.
        for (; pos > 8; pos -= 8)
        {
            retval += bits_in_byte(*bm++);
        }
        return retval + bits_in_byte(*bm & ((1 << pos) - 1));
    }

public:

    T& operator[](size_t i)
    {

        return 
    }

    


private:
    uint8_t bitmap[(GROUP_SIZE - 1) / 8 + 1]; // fancy math is so we round up

};


template<uint16_t GROUP_SIZE>
class BasicSparseTable
{

public:

private:
    std::vector<BasicSparseGroup> groups;
};

}

}

#endif