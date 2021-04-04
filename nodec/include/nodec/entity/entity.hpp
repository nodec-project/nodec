#ifndef NODEC__ENTITY__ENTITY_HPP_
#define NODEC__ENTITY__ENTITY_HPP_

#include <stdint.h>

namespace nodec
{
namespace entity
{

template<typename>
struct entity_traits;


/**
* @brief Entity traits for a 32 bits entity identifier.
*   A 32 bits entity identifier gurantees:
*
*   * 20 bits for entity number (suitable for almost all the games).
*   * 12 bits for the version (reset in [0-4095]).
*
*/
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

    static constexpr size_t entity_shift = 20u;

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

    /**
    * @biref Compares two null objects.
    */
    constexpr bool operator==(const NullEntity&) const noexcept
    {
        return true;
    }


    constexpr bool operator!=(const NullEntity&) const noexcept
    {
        return false;
    }


    /**
    * @brief Compares a null object and entity identifier of any type.
    */
    template<typename Entity>
    constexpr bool operator==(const Entity& entity) const noexcept
    {
        return (entity & entity_traits<Entity>::entity_mask) == static_cast<Entity>(*this);
    }


    template<typename Entity>
    constexpr bool operator!=(const Entity& entity) const noexcept
    {
        return !(entity == *this);
    }
};


/**
* @brief Compile-time constant for null entities.
*/
constexpr NullEntity null_entity{};


template<typename Entity>
typename entity_traits<Entity>::Version
get_version(const Entity entity)
{
    using Version = typename entity_traits<Entity>::Version;
    using traits = entity_traits<Entity>;
    return static_cast<Version>(entity >> traits::entity_shift);
}


using Entity = uint32_t;


}
}

#endif