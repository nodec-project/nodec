#ifndef NODEC__ENTITIES__REGISTRY_HPP_
#define NODEC__ENTITIES__REGISTRY_HPP_

#include "../formatter.hpp"
#include "../type_info.hpp"
#include "../utility.hpp"
#include "entity.hpp"
#include "exceptions.hpp"
#include "storage.hpp"
#include "view.hpp"

#include <cassert>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace nodec {
namespace entities {


template<typename Entity>
class BasicRegistry {
    using entt_traits = entity_traits<Entity>;

    template<typename Type>
    using Storage = storage_for_t<Type, Entity>;

    struct PoolData {
        std::unique_ptr<BaseStorage<Entity>> pool;
        const type_info* type_info;
    };

    decltype(auto) generate_identifier(const std::size_t pos) noexcept {
        assert(pos < entt_traits::to_integral(null_entity) && "No entities available");
        return entt_traits::combine(static_cast<typename entt_traits::entity_type>(pos), {});
    }

    decltype(auto) recycle_identifier() {
        assert(free_list != null_entity && "No entities available");
        const auto curr = entt_traits::to_entity(free_list);
        free_list = entt_traits::combine(entt_traits::to_integral(entities[curr]), tombstone_entity);
        return (entities[curr] = entt_traits::combine(curr, entt_traits::to_integral(entities[curr])));
    }

    decltype(auto) release_entity(const Entity entity, const typename entt_traits::version_type version) {
        const typename entt_traits::version_type vers = version + (version == entt_traits::to_version(tombstone_entity));
        entities[entt_traits::to_entity(entity)] = entt_traits::construct(entt_traits::to_integral(free_list), vers);
        free_list = entt_traits::combine(entt_traits::to_integral(entity), tombstone_entity);
        return vers;
    }

    template<typename Component>
    Storage<Component> *pool_assured() const {
        static_assert(std::is_same<Component, std::decay_t<Component>>::value, "Non-decayed types (s.t. array) not allowed");
        const auto index = type_id<Component>().seq_index();

        if (!(index < pools.size())) {
            pools.resize(index + 1u);
        }

        auto &&pdata = pools[index];
        if (!pdata.pool) {
            pdata.pool.reset(new Storage<Component>());
            pdata.type_info = &type_id<Component>();
        }

        return static_cast<Storage<Component> *>(pools[index].pool.get());
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
        static_assert(std::is_same<Component, std::decay_t<Component>>::value, "Non-decayed types (s.t. array) not allowed");
        const auto index = type_id<Component>().seq_index();
        return (index < pools.size() && pools[index].pool)
                   ? static_cast<Storage<Component> *>(pools[index].pool.get())
                   : nullptr;
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
        const auto pos = std::size_t(entt_traits::to_entity(entity));
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
            exceptions::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        remove_all_components(entity);
        release_entity(entity, entt_traits::to_version(entity) + 1u);
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
                if (entt_traits::to_entity(entity) == (pos - 1)) {
                    func(entity);
                }
            }
        }
    }

    void clear() {
        for (auto pos = pools.size(); pos; --pos) {
            auto &pdata = pools[pos - 1];
            if (pdata.pool) {
                pdata.pool->clear(*this);
            }
        }
        each_entity(
            [this](const auto entity) {
                release_entity(entity, entt_traits::to_version(entity) + 1u);
            });
    }

    template<typename Component, typename... Args>
    decltype(auto) emplace_component(const Entity entity, Args &&...args) {
        if (!is_valid(entity)) {
            exceptions::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        return pool_assured<Component>()->emplace(*this, entity, std::forward<Args>(args)...);
    }
    
    template<typename Component>
    bool remove_component(const Entity entity) {
        if (!is_valid(entity)) {
            exceptions::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        auto *cpool = pool_if_exists<Component>();
        return cpool != nullptr && cpool->erase(*this, entity);
    }

    template<typename... Components>
    decltype(auto) remove_components(const Entity entity) {
        static_assert(sizeof...(Components) > 0, "Must provide one or more component types");

        if (!is_valid(entity)) {
            exceptions::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        return std::make_tuple(([this, entity](auto *cpool) {
            return cpool != nullptr && cpool->erase(*this, entity);
        })(pool_if_exists<Components>())...);
    }

    void remove_all_components(const Entity entity) {
        if (!is_valid(entity)) {
            exceptions::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        for (auto pos = pools.size(); pos; --pos) {
            auto &pdata = pools[pos - 1];
            if (pdata.pool) {
                pdata.pool->erase(*this, entity);
            }
        }
    }

    template<typename Component>
    decltype(auto) get_component(const Entity entity) const {
        if (!is_valid(entity)) {
            exceptions::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        using Comp = std::remove_const_t<Component>;
        const auto *cpool = pool_if_exists<Comp>();
        if (!cpool) {
            exceptions::throw_no_component_exception<Comp>(entity, __FILE__, __LINE__);
        }

        auto *component = cpool->try_get(entity);
        if (!component) {
            exceptions::throw_no_component_exception<Comp>(entity, __FILE__, __LINE__);
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
            exceptions::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }
        auto *cpool = pool_if_exists<std::remove_const_t<Component>>();
        return cpool ? cpool->try_get(entity) : nullptr;
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
            const auto &pdata = pools[pos - 1];
            if (!pdata.pool) continue;

            auto component = pdata.pool->try_get_opaque(entity);
            if (!component) continue;

            func(*pdata.type_info, component);
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
    mutable std::vector<PoolData> pools{};
    std::vector<Entity> entities;
    Entity free_list{tombstone_entity};
};

using Registry = BasicRegistry<Entity>;

} // namespace entities
} // namespace nodec

#endif