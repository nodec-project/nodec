#ifndef NODEC__ENTITY__ENTITY_HPP_
#define NODEC__ENTITY__ENTITY_HPP_

#include <cstdint>


namespace nodec
{
namespace entity
{

template<typename>
struct entity_traits;


template<>
struct entity_traits<std::uint32_t>
{
    /**
    * @brief Underlying entity type.
    */
    using EntityT = std::uint32_t;


    using VersionT = std::uint16_t;



};

struct NullEntity
{
    /**
    * @brief Cast the null object to identifiers of any type.
    */
    template<typename EntityT>
    constexpr operator EntityT() const noexcept
    {
    }
};

}
}

#endif