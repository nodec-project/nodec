#ifndef NODEC__ENTITIES__REGISTRY_HPP_
#define NODEC__ENTITIES__REGISTRY_HPP_

#include "../formatter.hpp"
#include "../type_info.hpp"
#include "../utility.hpp"
#include "entity.hpp"
#include "exceptions.hpp"
#include "storage.hpp"
#include "view.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace nodec {
namespace entities {

template<typename Entity>
class BasicRegistry {
    template<typename Type>
    using Storage = storage_for_t<Entity, Type>;

    struct PoolData {
        std::unique_ptr<BaseStorage<Entity>> pool;
        const type_info *type_info;
    };

public:
    using entity_traits_type = entity_traits<Entity>;
    using entity_type = Entity;

private:
    decltype(auto) generate_identifier(const std::size_t pos) noexcept {
        assert(pos < entity_traits_type::to_integral(null_entity) && "No entities available");
        return entity_traits_type::combine(static_cast<typename entity_traits_type::entity_type>(pos), {});
    }

    decltype(auto) recycle_identifier() {
        assert(free_list != null_entity && "No entities available");
        const auto curr = entity_traits_type::to_entity(free_list);
        free_list = entity_traits_type::combine(entity_traits_type::to_integral(entities[curr]), tombstone_entity);
        return (entities[curr] = entity_traits_type::combine(curr, entity_traits_type::to_integral(entities[curr])));
    }

    decltype(auto) release_entity(const Entity entity, const typename entity_traits_type::version_type version) {
        const typename entity_traits_type::version_type vers = version + (version == entity_traits_type::to_version(tombstone_entity));
        entities[entity_traits_type::to_entity(entity)] = entity_traits_type::construct(entity_traits_type::to_integral(free_list), vers);
        free_list = entity_traits_type::combine(entity_traits_type::to_integral(entity), tombstone_entity);
        return vers;
    }

    template<typename Component>
    Storage<Component> *pool_assured() {
        static_assert(std::is_same<Component, std::decay_t<Component>>::value, "Non-decayed types (s.t. array) not allowed");
        const auto index = type_id<Component>().seq_index();

        if (!(index < pools.size())) {
            pools.resize(index + 1u);
        }

        auto &&pool_data = pools[index];
        if (!pool_data.pool) {
            pool_data.pool.reset(new Storage<Component>());
            pool_data.pool->bind_registry(const_cast<BasicRegistry *>(this));
            pool_data.type_info = &type_id<Component>();
        }

        return static_cast<Storage<Component> *>(pools[index].pool.get());
    }

    template<typename Component>
    const Storage<Component> *pool_assured() const {
        static_assert(std::is_same<Component, std::decay_t<Component>>::value, "Non-decayed types (s.t. array) not allowed");

        const auto index = type_id<Component>().seq_index();
        if (index < pools.size() && pools[index].pool) {
            return static_cast<const Storage<Component> *>(pools[index].pool.get());
        }

        static Storage<Component> placeholder{};
        return &placeholder;
    }

    template<typename Component>
    const Storage<Component> *pool_if_exists() const {
        static_assert(std::is_same<Component, std::decay_t<Component>>::value, "Non-decayed types (s.t. array) not allowed");
        const auto index = type_id<Component>().seq_index();
        return (index < pools.size() && pools[index].pool)
                   ? static_cast<const Storage<Component> *>(pools[index].pool.get())
                   : nullptr;
    }

    template<typename Component>
    Storage<Component> *pool_if_exists() {
        return const_cast<Storage<Component> *>(as_const(*this).template pool_if_exists<Component>());
    }

public:
    /**
     * @brief Default constructor.
     */
    BasicRegistry() = default;

    /**
     * @brief Default move constructor.
     */
    BasicRegistry(BasicRegistry &&) = default;

    /**
     * @brief Default move assignment operator.
     * @return This registry.
     */
    BasicRegistry &operator=(BasicRegistry &&) = default;

    /**
     * @brief Checks if an identifier refers to a valid entity.
     * @param entity An identifier, either valid or not.
     * @return True if the identifier is valid, false otherwise.
     */
    bool is_valid(const Entity entity) const {
        const auto pos = std::size_t(entity_traits_type::to_entity(entity));
        return (pos < entities.size() && entities[pos] == entity);
    }

    /**
     * @brief Creates a new entity and returns it.
     *
     * There are two kinds of possible identifiers:
     *
     * * Newly created ones in case no entities have been previously destroyed.
     * * Recycled ones with updated versions.
     *
     * @return A valid identifier.
     */
    Entity create_entity() {
        return (free_list == null_entity) ? (entities.emplace_back(generate_identifier(entities.size())), entities.back()) : recycle_identifier();
    }

    /**
     * @brief Destroys an entity.
     *   When an entity is destroyed, its version is updated and the identifier
     *   can be recycled at any time.
     *
     * @param entity
     *   A valid entity identifier.
     */
    void destroy_entity(const Entity entity) {
        if (!is_valid(entity)) {
            throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        remove_all_components(entity);
        release_entity(entity, entity_traits_type::to_version(entity) + 1u);
    }

    /**
     * @brief Destroys all entities in a range and releases their identifiers.
     */
    template<typename It>
    void destroy_entities(It first, It last) {
        for (; first != last; ++first) {
            destroy_entity(*first);
        }
    }

    /**
     * @brief Iterates all the entities that are still in use.
     *
     * The function object is invoked for each entity that is still in use.<br/>
     * The signature of the function should be equivalent to the following:
     *
     * @code{.cpp}
     * void(const Entity);
     * @endcode
     *
     * This function is fairly slow and should not be used frequently. However,
     * it's useful for iterating all the entities still in use, regardless of
     * their components.
     *
     * @tparam Func Type of the function object to invoke.
     * @param func A valid function object.
     */
    template<typename Func>
    void each_entity(Func func) const {
        if (free_list == null_entity) {
            for (auto pos = entities.size(); pos; --pos) {
                func(entities[pos - 1]);
            }
        } else {
            for (auto pos = entities.size(); pos; --pos) {
                const auto entity = entities[pos - 1];
                if (entity_traits_type::to_entity(entity) == (pos - 1)) {
                    func(entity);
                }
            }
        }
    }

    /**
     * @brief Check if an entity is part of all the given component.
     */
    template<typename... Components>
    bool all_of(const Entity entity) const {
        // NOTE: If over C++17, we can use fold expression.
        //  return (pool_assured<std::remove_const_t<Type>>().contains(entt) && ...);

        std::array<bool, sizeof...(Components)> values{pool_assured<std::remove_const_t<Components>>()->contains(entity)...};
        return std::all_of(values.cbegin(), values.cend(), [](auto value) { return value; });
    }

    /**
     * @brief Check if an entity is part of at least one given component.
     */
    template<typename... Components>
    bool any_of(const Entity entity) const {
        // NOTE: If over C++17, we can use fold expression.
        //  return (pool_assured<std::remove_const_t<Type>>().contains(entt) || ...);

        std::array<bool, sizeof...(Components)> values{pool_assured<std::remove_const_t<Components>>()->contains(entity)...};
        return std::any_of(values.cbegin(), values.cend(), [](auto value) { return value; });
    }

    void clear() {
        for (auto pos = pools.size(); pos; --pos) {
            auto &pool_data = pools[pos - 1];
            if (pool_data.pool) {
                pool_data.pool->clear();
            }
        }
        each_entity(
            [this](const auto entity) {
                release_entity(entity, entity_traits_type::to_version(entity) + 1u);
            });
    }

    template<typename Component, typename... Args>
    decltype(auto) emplace_component(const Entity entity, Args &&...args) {
        if (!is_valid(entity)) {
            throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        return pool_assured<Component>()->emplace(entity, std::forward<Args>(args)...);
    }

    template<typename Component>
    bool remove_component(const Entity entity) {
        auto *pool = pool_if_exists<Component>();
        return pool != nullptr && pool->erase(entity);
    }

    template<typename... Components>
    decltype(auto) remove_components(const Entity entity) {
        static_assert(sizeof...(Components) > 0, "Must provide one or more component types");

        return std::make_tuple(([this, entity](auto *pool) {
            return pool != nullptr && pool->erase(entity);
        })(pool_if_exists<Components>())...);
    }

    void remove_all_components(const Entity entity) {
        for (auto pos = pools.size(); pos; --pos) {
            auto &pool_date = pools[pos - 1];
            if (pool_date.pool) {
                pool_date.pool->erase(entity);
            }
        }
    }

    /**
     * @brief Removes the given components from all the entities in a range.
     *
     * @return std::size_t The number of components actually removed.
     */
    template<typename Component, typename It>
    std::size_t remove_component(It first, It last) {
        auto *pool = pool_if_exists<Component>();
        if (!pool) return 0;

        return pool->erase(first, last);
    }

    template<typename... Components, typename It>
    decltype(auto) remove_components(It first, It last) {
        return std::forward_as_tuple(remove_component<Components>(first, last)...);
    }

    template<typename Component>
    void clear_component() {
        auto *pool = pool_if_exists<Component>();
        if (pool == nullptr) return;
        pool->clear();
    }

    template<typename Component>
    decltype(auto) get_component(const Entity entity) const {
        if (!is_valid(entity)) {
            throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        using Comp = std::remove_const_t<Component>;
        const auto *pool = pool_if_exists<Comp>();
        if (!pool) {
            throw_no_component_exception<Comp>(entity, __FILE__, __LINE__);
        }

        auto *component = pool->try_get(entity);
        if (!component) {
            throw_no_component_exception<Comp>(entity, __FILE__, __LINE__);
        }

        return *component;
    }

    template<typename Component>
    decltype(auto) get_component(const Entity entity) {
        return const_cast<Component &>(nodec::as_const(*this).template get_component<Component>(entity));
    }

    template<typename... Components>
    decltype(auto) get_components(const Entity entity) const {
        return std::forward_as_tuple(get_component<Components>(entity)...);
    }

    template<typename... Components>
    decltype(auto) get_components(const Entity entity) {
        return std::forward_as_tuple(get_component<Components>(entity)...);
    }

    template<typename Component>
    auto try_get_component(const Entity entity) const {
        if (!is_valid(entity)) {
            throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }
        auto *pool = pool_if_exists<std::remove_const_t<Component>>();
        return pool ? pool->try_get(entity) : nullptr;
    }

    template<typename Component>
    Component *try_get_component(const Entity entity) {
        return const_cast<Component *>(nodec::as_const(*this).template try_get_component<Component>(entity));
    }

    template<typename... Components>
    decltype(auto) try_get_components(const Entity entity) const {
        return std::make_tuple(try_get_component<Components>(entity)...);
    }

    template<typename... Components>
    decltype(auto) try_get_components(const Entity entity) {
        return std::make_tuple(try_get_component<Components>(entity)...);
    }

    /**
     * @brief Returns a view for the given components.
     *
     * @tparam Type Type of component used to construct the view.
     * @tparam Others Other types of components used to construct the view.
     * @tparam Exclusions Types of components used to filter the view.
     * @return A newly created view.
     */
    template<typename Type, typename... Others, typename... Exclusions>
    BasicView<type_list<Storage<const Type>, Storage<const Others>...>, type_list<Storage<const Exclusions>...>>
    view(type_list<Exclusions...> = {}) const {
        return {
            *pool_assured<std::remove_const_t<Type>>(),
            *pool_assured<std::remove_const_t<Others>>()...,
            *pool_assured<std::remove_const_t<Exclusions>>()...};
    }

    /*! @copydoc view */
    template<typename Type, typename... Others, typename... Exclusions>
    BasicView<type_list<Storage<Type>, Storage<Others>...>, type_list<Storage<Exclusions>...>>
    view(type_list<Exclusions...> = {}) {
        return {
            *pool_assured<std::remove_const_t<Type>>(),
            *pool_assured<std::remove_const_t<Others>>()...,
            *pool_assured<std::remove_const_t<Exclusions>>()...};
    }

    /**
     * @brief Visits an entity and returns the type seq index and opaque pointer for its components.
     *
     * The signature of the function should be equivalent to the following:
     *
     * @code{.cpp}
     * void(const type_info&, void* component);
     * @endcode
     */
    template<typename Func>
    void visit(Entity entity, Func func) const {
        for (auto pos = pools.size(); pos; --pos) {
            const auto &pool_data = pools[pos - 1];
            if (!pool_data.pool) continue;

            auto component = pool_data.pool->try_get_opaque(entity);
            if (!component) continue;

            func(*pool_data.type_info, component);
        }
    }

    /**
     * @brief Returns signal interface for the given component.
     */
    template<typename Component>
    decltype(auto) component_constructed() {
        return pool_assured<Component>()->element_constructed();
    }

    /**
     * @brief Returns signal interface for the given component.
     */
    template<typename Component>
    decltype(auto) component_destroyed() {
        return pool_assured<Component>()->element_destroyed();
    }

private:
    std::vector<PoolData> pools{};
    std::vector<Entity> entities;
    Entity free_list{tombstone_entity};
};

using Registry = BasicRegistry<Entity>;

} // namespace entities
} // namespace nodec

#endif