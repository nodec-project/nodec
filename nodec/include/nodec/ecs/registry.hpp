#ifndef NODEC__ECS__REGISTRY_HPP_
#define NODEC__ECS__REGISTRY_HPP_

#include <nodec/ecs/entity.hpp>
#include <nodec/ecs/storage.hpp>
#include <nodec/ecs/type_info.hpp>
#include <nodec/nodec_exception.hpp>

#include <sstream>
#include <memory>
#include <vector>


namespace nodec
{
namespace ecs
{

class InvalidEntityException : public NodecException
{
public:
    template<typename Entity>
    InvalidEntityException(Entity entity, const char* file, size_t line)
        :NodecException(file, line)
    {
        std::ostringstream oss;
        oss << "Invalid entity detected. entity: " << entity 
            << "(position: " << (entity_traits<Entity>::entity_mask & entity) << "; version: " << get_version(entity) << ")";
        message = oss.str();
    }
};

template<typename Entity>
class BasicRegistry
{
private:
    using entity_traits = entity_traits<Entity>;

    struct PoolData
    {
        std::unique_ptr<BaseStorage<Entity>> pool;
    };



    Entity generate_identifier()
    {
        auto generated = static_cast<Entity>(entities.size());
        entities.emplace_back(generated);
        return generated;
    }

    Entity recycle_identifier()
    {
        const auto curr = available;
        const auto version = entities[curr] & (entity_traits::version_mask << entity_traits::entity_shift);
        available = entities[curr] & entity_traits::entity_mask;
        return entities[curr] = curr | version;
    }

    void release_entity(const Entity entity, const typename entity_traits::Version version)
    {
        const auto entt = entity & entity_traits::entity_mask;
        entities[entt] = available | (static_cast<Entity>(version) << entity_traits::entity_shift);
        available = entt;
    }

    template<typename Component>
    BasicStorage<Entity, Component>* which_pool()
    {
        static_assert(std::is_same_v<Component, std::decay_t<Component>>, "Non-decayed types (s.t. array) not allowed");
        const auto index = type_seq<Component>::value();

        if (!(index < pools.size()))
        {
            pools.resize(index + 1);
        }

        auto&& pdata = pools[index];
        if (!pdata.pool)
        {
            pdata.pool.reset(new BasicStorage<Entity, Component>());
        }

        return static_cast<BasicStorage<Entity, Component>*>(pools[index].pool.get());
    }

public:


    /**
    * @brief Checks if an entity identifier refers to a valid entity.
    * @param entity
    *   An entity identifier, either valid or not.
    * @return True if the identifier is valid, false otherwise.
    */
    bool is_valid(const Entity entity) const
    {
        const auto pos = static_cast<size_t>(entity & entity_traits::entity_mask);
        return (pos < entities.size() && entities[pos] == entity);
    }


    Entity create_entity()
    {
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
    void destroy_entity(const Entity entity)
    {
        if (!is_valid(entity))
        {
            throw InvalidEntityException(entity, __FILE__, __LINE__);
        }

        release_entity(entity, static_cast<typename entity_traits::Version>(get_version(entity) + 1));
    }


    template<typename Component, typename... Args>
    Component& add_component(const Entity entity, Args &&... args)
    {
        if (!is_valid(entity))
        {
            throw InvalidEntityException(entity, __FILE__, __LINE__);
        }

        return which_pool<Component>()->emplace(entity, std::forward<Args>(args)...);
    }



    template<typename... Components>
    void remove_components(const Entity entity)
    {
        static_assert(sizeof...(Components) > 0, "Must provide one or more component types");

    }




private:
    std::vector<Entity> entities;
    Entity available{ null_entity };
    std::vector<PoolData> pools{};


};

using Registry = BasicRegistry<Entity>;

}
}

#endif