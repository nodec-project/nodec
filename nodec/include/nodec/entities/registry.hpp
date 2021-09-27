#ifndef NODEC__ENTITIES__REGISTRY_HPP_
#define NODEC__ENTITIES__REGISTRY_HPP_

#include <nodec/entities/entity.hpp>
#include <nodec/entities/storage.hpp>
#include <nodec/entities/view.hpp>
#include <nodec/error_formatter.hpp>
#include <nodec/type_info.hpp>

#include <stdexcept>
#include <sstream>
#include <memory>
#include <vector>


namespace nodec {
namespace entities {


namespace details {

template<typename Entity>
inline void throw_invalid_entity_exception(const Entity entity, const char* file, size_t line) {
    throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
        Formatter() << "Invalid entity detected. entity: " << entity
        << "(position: " << (entity_traits<Entity>::entity_mask & entity) << "; version: " << get_version(entity) << ")",
        file, line
        ));
}

template<typename Component, typename Entity>
inline void throw_no_component_exception(const Entity entity, const char* file, size_t line) {
    throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
        Formatter() << "Entity(" << entity << "; position: "
        << (entity_traits<Entity>::entity_mask & entity) << "; version: " << get_version(entity)
        << ") doesn't have the component(" << typeid(Component).name() << ").",
        file, line
        ));
}

}


template<typename Entity>
class BasicRegistry {
private:
    using entity_traits = entity_traits<Entity>;

    struct PoolData {
        std::unique_ptr<BaseStorage<Entity>> pool;
    };


    Entity generate_identifier() {
        auto generated = static_cast<Entity>(entities.size());
        entities.emplace_back(generated);
        return generated;
    }

    Entity recycle_identifier() {
        const auto curr = available;
        const auto version = entities[curr] & (entity_traits::version_mask << entity_traits::entity_shift);
        available = entities[curr] & entity_traits::entity_mask;
        return entities[curr] = curr | version;
    }

    void release_entity(const Entity entity, const typename entity_traits::Version version) {
        const auto entt = entity & entity_traits::entity_mask;
        entities[entt] = available | (static_cast<Entity>(version) << entity_traits::entity_shift);
        available = entt;
    }

    template<typename Component>
    BasicStorage<Entity, Component>* pool_assured() const {
        static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types (s.t. array) not allowed");
        const auto index = type_seq<Component>::value();

        if (!(index < pools.size())) {
            pools.resize(index + 1u);
        }

        auto&& pdata = pools[index];
        if (!pdata.pool) {
            pdata.pool.reset(new BasicStorage<Entity, Component>());
        }

        return static_cast<BasicStorage<Entity, Component>*>(pools[index].pool.get());
    }

    template<typename Component>
    const BasicStorage<Entity, Component>* pool_if_exists() const {
        static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types (s.t. array) not allowed");
        const auto index = type_seq<Component>::value();
        return (index < pools.size() && pools[index].pool)
            ? static_cast<const BasicStorage<Entity, Component>*>(pools[index].pool.get())
            : nullptr;
    }

    template<typename Component>
    BasicStorage<Entity, Component>* pool_if_exists() {
        static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types (s.t. array) not allowed");
        const auto index = type_seq<Component>::value();
        return (index < pools.size() && pools[index].pool)
            ? static_cast<BasicStorage<Entity, Component>*>(pools[index].pool.get())
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
    BasicRegistry(BasicRegistry&&) = default;

    /**
    * @brief Default move assignment operator.
    * @return This registry.
    */
    BasicRegistry& operator=(BasicRegistry&&) = default;

    /**
    * @brief Checks if an entity identifier refers to a valid entity.
    * @param entity
    *   An entity identifier, either valid or not.
    * @return True if the identifier is valid, false otherwise.
    */
    bool is_valid(const Entity entity) const {
        const auto pos = static_cast<size_t>(entity & entity_traits::entity_mask);
        return (pos < entities.size() && entities[pos] == entity);
    }


    Entity create_entity() {
        return available == null_entity ? generate_identifier() : recycle_identifier();
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
            details::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        remove_all_components(entity);
        release_entity(entity, static_cast<typename entity_traits::Version>(get_version(entity) + 1));
    }

    template<typename Func>
    void each_entity(Func func) const {
        if (available == null_entity) {
            for (auto pos = entities.size(); pos; --pos) {
                func(entities[pos - 1]);
            }
        }
        else {
            for (auto pos = entities.size(); pos; --pos) {
                const auto entity = entities[pos - 1];
                if ((entity & entity_traits::entity_mask) == (pos - 1)) {
                    func(entity);
                }
            }
        }
    }

    void clear() {
        for (auto pos = pools.size(); pos; --pos) {
            auto& pdata = pools[pos - 1];
            if (pdata.pool) {
                pdata.pool->clear(*this);
            }
        }
        each_entity(
            [this](const auto entity) {
                release_entity(entity, get_version(entity) + 1u);
            });
    }

    template<typename Component, typename... Args>
    decltype(auto) emplace_component(const Entity entity, Args &&... args) {
        if (!is_valid(entity)) {
            details::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        return pool_assured<Component>()->emplace(*this, entity, std::forward<Args>(args)...);
    }

    template<typename... Components>
    decltype(auto) remove_components(const Entity entity) {
        static_assert(sizeof...(Components) > 0, "Must provide one or more component types");

        if (!is_valid(entity)) {
            details::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        return std::make_tuple(([entity](auto* cpool) {
            return cpool != nullptr && cpool->erase(*this, entity);
                                })(pool_if_exists<Components>())...);
    }

    void remove_all_components(const Entity entity) {
        if (!is_valid(entity)) {
            details::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        for (auto pos = pools.size(); pos; --pos) {
            auto& pdata = pools[pos - 1];
            if (pdata.pool) {
                pdata.pool->erase(*this, entity);
            }
        }
    }

    template<typename Component>
    decltype(auto) get_component(const Entity entity) const {
        if (!is_valid(entity)) {
            details::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }

        using Comp = std::remove_const_t<Component>;
        auto* cpool = pool_if_exists<Comp>();
        if (!cpool) {
            details::throw_no_component_exception<Comp>(entity, __FILE__, __LINE__);
        }

        auto* component = cpool->try_get(entity);
        if (!component) {
            details::throw_no_component_exception<Comp>(entity, __FILE__, __LINE__);
        }

        return *component;
    }

    template<typename Component>
    decltype(auto) get_component(const Entity entity) {
        return const_cast<Component&>(std::as_const(*this).get_component<Component>(entity));
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
            details::throw_invalid_entity_exception(entity, __FILE__, __LINE__);
        }
        auto* cpool = pool_if_exists<std::remove_const_t<Component>>();
        return cpool ? cpool->try_get(entity) : nullptr;
    }

    template<typename Component>
    Component* try_get_component(const Entity entity) {
        return const_cast<Component*>(std::as_const(*this).try_get_component<Component>(entity));
    }


    template<typename... Components>
    decltype(auto) try_get_components(const Entity entity) const {
        return std::make_tuple(try_get_component<Components>(entity)...);
    }

    template<typename... Components>
    decltype(auto) try_get_components(const Entity entity) {
        return std::make_tuple(try_get_component<Components>(entity)...);
    }

    template<typename... Components>
    BasicView<Entity, Components...> view() const {
        static_assert((std::min)({ std::is_const_v<Components> ... }), "Invalid non-const type");
        //static_assert((std::is_const_v<Components> && ...), "Invalid non-const type"); // For over C++17

        return { *pool_assured<std::remove_const_t<Components>>()... };
    }

    template<typename... Components>
    BasicView<Entity, Components...> view() {
        return { *pool_assured<std::remove_const_t<Components>>()... };
    }


    template<typename Component>
    decltype(auto) component_constructed() {
        return pool_assured<Component>()->element_constructed();
    }


    template<typename Component>
    decltype(auto) component_destroyed() {
        return pool_assured<Component>()->element_destroyed();
    }


private:
    std::vector<Entity> entities;
    Entity available{ null_entity };
    mutable std::vector<PoolData> pools{};
};

using Registry = BasicRegistry<Entity>;

}
}

#endif