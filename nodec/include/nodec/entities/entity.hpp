#ifndef NODEC__ENTITIES__ENTITY_HPP_
#define NODEC__ENTITIES__ENTITY_HPP_

#include <cstddef>
#include <cstdint>

namespace nodec {
namespace entities {

namespace internal {

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
struct entity_traits<std::uint32_t> {
    // like-stl.
    using entity_type = std::uint32_t;
    using version_type = std::uint16_t;

    static constexpr entity_type entity_mask = 0xFFFFF;
    static constexpr entity_type version_mask = 0xFFF;
    static constexpr std::size_t entity_shift = 20u;
};

template<>
struct entity_traits<std::uint64_t> {
    using entity_type = std::uint64_t;
    using version_type = std::uint32_t;

    static constexpr entity_type entity_mask = 0xFFFFFFFF;
    static constexpr entity_type version_mask = 0xFFFFFFFF;
    static constexpr std::size_t entity_shift = 32u;
};

} // namespace internal

template<typename Type>
class entity_traits : internal::entity_traits<Type> {
    using base_type = internal::entity_traits<Type>;

public:
    // like-stl.

    /*! @brief Value type. */
    using value_type = Type;
    /*! @brief Underlying entity type. */
    using entity_type = typename base_type::entity_type;
    /*! @brief Underlying version type. */
    using version_type = typename base_type::version_type;
    /*! @brief Reserved identifier. */
    static constexpr entity_type reserved = base_type::entity_mask | (base_type::version_mask << base_type::entity_shift);

    /**
     * @brief Converts an entity to its underlying type.
     * @param value The value to convert.
     * @return The integral representation of the given value.
     */
    static constexpr entity_type to_integral(const value_type value) noexcept {
        return static_cast<entity_type>(value);
    }

    /**
     * @brief Returns the entity part once converted to the underlying type.
     * @param value The value to convert.
     * @return The integral representation of the entity part.
     */
    static constexpr entity_type to_entity(const value_type value) noexcept {
        return (to_integral(value) & base_type::entity_mask);
    }

    /**
     * @brief Returns the version part once converted to the underlying type.
     * @param value The value to convert.
     * @return The integral representation of the version part.
     */
    static constexpr version_type to_version(const value_type value) noexcept {
        constexpr auto version_mask = (base_type::version_mask << base_type::entity_shift);
        return ((to_integral(value) & version_mask) >> base_type::entity_shift);
    }

    /**
     * @brief Constructs an identifier from its parts.
     *
     * If the version part is not provided, a tombstone is returned.<br/>
     * If the entity part is not provided, a null identifier is returned.
     *
     * @param entity The entity part of the identifier.
     * @param version The version part of the identifier.
     * @return A properly constructed identifier.
     */
    static constexpr value_type construct(const entity_type entity, const version_type version) noexcept {
        return value_type{(entity & base_type::entity_mask) | (static_cast<entity_type>(version) << base_type::entity_shift)};
    }

    /**
     * @brief Combines two identifiers in a single one.
     *
     * The returned identifier is a copy of the first element except for its
     * version, which is taken from the second element.
     *
     * @param lhs The identifier from which to take the entity part.
     * @param rhs The identifier from which to take the version part.
     * @return A properly constructed identifier.
     */
    static constexpr value_type combine(const entity_type lhs, const entity_type rhs) noexcept {
        constexpr auto version_mask = (base_type::version_mask << base_type::entity_shift);
        return value_type{(lhs & base_type::entity_mask) | (rhs & version_mask)};
    }
};

template<typename Entity>
constexpr typename entity_traits<Entity>::entity_type to_integral(const Entity value) noexcept {
    return entity_traits<Entity>::to_integral(value);
}

template<typename Entity>
constexpr typename entity_traits<Entity>::entity_type to_entity(const Entity value) noexcept {
    return entity_traits<Entity>::to_entity(value);
}

template<typename Entity>
constexpr typename entity_traits<Entity>::version_type to_version(const Entity value) noexcept {
    return entity_traits<Entity>::to_version(value);
}

struct NullEntity {
    /**
     * @brief Converts the null object to identifiers of any type.
     * @tparam Entity Type of identifier.
     * @return The null representation for the given type.
     */
    template<typename Entity>
    constexpr operator Entity() const noexcept {
        using entt_traits = entity_traits<Entity>;
        return entt_traits::combine(entt_traits::reserved, entt_traits::reserved);
    }

    /**
     * @brief Compares two null objects.
     * @param other A null object.
     * @return True in all cases.
     */
    constexpr bool operator==(const NullEntity other) const noexcept {
        return true;
    }

    /**
     * @brief Compares two null objects.
     * @param other A null object.
     * @return False in all cases.
     */
    constexpr bool operator!=(const NullEntity other) const noexcept {
        return false;
    }

    /**
     * @brief Compares a null object and an identifier of any type.
     * @tparam Entity Type of identifier.
     * @param entity Identifier with which to compare.
     * @return False if the two elements differ, true otherwise.
     */
    template<typename Entity>
    constexpr bool operator==(const Entity entity) const noexcept {
        using entt_traits = entity_traits<Entity>;
        return entt_traits::to_entity(entity) == entt_traits::to_entity(*this);
    }

    /**
     * @brief Compares a null object and an identifier of any type.
     * @tparam Entity Type of identifier.
     * @param entity Identifier with which to compare.
     * @return True if the two elements differ, false otherwise.
     */
    template<typename Entity>
    constexpr bool operator!=(const Entity entity) const noexcept {
        return !(entity == *this);
    }
};

/**
 * @brief Compares a null object and an identifier of any type.
 * @tparam Entity Type of identifier.
 * @param entity Identifier with which to compare.
 * @param other A null object yet to be converted.
 * @return False if the two elements differ, true otherwise.
 */
template<typename Entity>
constexpr bool operator==(const Entity entity, const NullEntity other) noexcept {
    return other.operator==(entity);
}

/**
 * @brief Compares a null object and an identifier of any type.
 * @tparam Entity Type of identifier.
 * @param entity Identifier with which to compare.
 * @param other A null object yet to be converted.
 * @return True if the two elements differ, false otherwise.
 */
template<typename Entity>
constexpr bool operator!=(const Entity entity, const NullEntity other) noexcept {
    return other.operator!=(entity);
}

struct TombstoneEntity {
    /**
     * @brief Converts the tombstone object to identifiers of any type.
     * @tparam Entity Type of identifier.
     * @return The tombstone representation for the given type.
     */
    template<typename Entity>
    constexpr operator Entity() const noexcept {
        using entt_traits = entity_traits<Entity>;
        return entt_traits::combine(entt_traits::reserved, entt_traits::reserved);
    }

    /**
     * @brief Compares two tombstone objects.
     * @param other A tombstone object.
     * @return True if all cases.
     */
    constexpr bool operator==(const TombstoneEntity other) const noexcept {
        return true;
    }

    /**
     * @brief Compares two tombstone objects.
     * @param other A tombstone object.
     * @return False in all cases.
     */
    constexpr bool operator!=(const TombstoneEntity other) const noexcept {
        return false;
    }

    /**
     * @brief Compares a tombstone object and an identifier of any type.
     * @tparam Entity Type of identifier.
     * @param entity Identifier with which to compare.
     * @return False if the two elements differ, true otherwise.
     */
    template<typename Entity>
    constexpr bool operator==(const Entity entity) const noexcept {
        using entt_traits = entity_traits<Entity>;
        return entt_traits::to_version(entity) == entt_traits::to_version(*this);
    }

    /**
     * @brief Compares a tombstone object and an identifier of any type.
     * @tparam Entity Type of identifier.
     * @param entity Identifier with which to compare.
     * @return True if the two elements differ, false otherwise.
     */
    template<typename Entity>
    constexpr bool operator!=(const Entity entity) const noexcept {
        return !(entity != *this);
    }
};

/**
 * @brief Compares a tombstone object and an identifier of any type.
 * @tparam Entity Type of identifier.
 * @param entity Identifier with which to compare.
 * @param other A tombstone object yet to be converted.
 * @return False if the two elements differ, true otherwise.
 */
template<typename Entity>
constexpr bool operator==(const Entity entity, const TombstoneEntity other) noexcept {
    return other.operator==(entity);
}

/**
 * @brief Compares a tombstone object and an identifier of any type.
 * @tparam Entity Type of identifier.
 * @param entity Identifier with which to compare.
 * @param other A tombstone object yet to be converted.
 * @return True if the two elements differ, false otherwise.
 */
template<typename Entity>
constexpr bool operator!=(const Entity entity, const TombstoneEntity other) noexcept {
    return !(other == entity);
}

/**
 * @brief Compile-time constant for null entities.
 *
 * There exist implicit conversions from this variable to identifiers of any
 * allowed type. Similarly, there exist comparision operators between the null
 * entity and any other identifier.
 */
constexpr NullEntity null_entity{};

/**
 * @brief Compile-time constant for tombstone entities.
 *
 * There exist implicit conversions from this variable to identifiers of any
 * allowed type. Similarly, there exist comparision operators between the
 * tombstone entity and any other identifier.
 */
constexpr TombstoneEntity tombstone_entity{};

using Entity = uint32_t;

} // namespace entities
} // namespace nodec

#endif