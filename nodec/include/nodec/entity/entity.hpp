#ifndef NODEC__ENTITY__ENTITY_HPP_
#define NODEC__ENTITY__ENTITY_HPP_

#include <stdint.h>

namespace nodec
{
namespace entity
{

template<typename>
struct entity_traits;


template<>
struct entity_traits<uint32_t>
{
    /**
    * @brief Underlying entity type.
    */
    using Entity = uint32_t;


    using Version = uint16_t;

    static constexpr Entity entity_mask = 0xFFFFF;

    static constexpr Entity version_mask = 0xFFF;

};


struct NullEntity
{
    /**
    * @brief Cast the null object to identifiers of any type.
    */
    template<typename Entity>
    constexpr operator Entity() const noexcept
    {
        return Entity{ entity_traits<Entity>::entity_mask };
    }

};

/**
* @brief Compile-time constant for null entities.
*/
constexpr NullEntity null_entity{};



}
}

#endif