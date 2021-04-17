#ifndef NODEC__ENTITY__REGISTRY_HPP_
#define NODEC__ENTITY__REGISTRY_HPP_

#include <nodec/entity/entity.hpp>
#include <nodec/entity/storage.hpp>

#include <memory>
#include <vector>


namespace nodec
{
namespace entity
{

template<typename EntityT>
class BasicRegistry
{
private:
    using entity_traits = entity_traits<EntityT>;

    struct PoolData
    {
        std::unique_ptr<BaseStorage<EntityT>> pool;
    };



    EntityT generate_identifier()
    {
        return entities.emplace_back(static_cast<EntityT>(entities.size()));
    }

    EntityT recycle_identifier()
    {
        const auto curr = available;
        const auto version = entities[curr] & (entity_traits::version_mask << entity_traits::entity_shift);
        available = entities[curr] & entity_traits::entity_mask;
        return entities[curr] = curr | version;
    }

    void release_entity(const EntityT entity, const typename entity_traits::VersionT version)
    {
        const auto entt = entity & entity_traits::entity_mask;
        entities[entt] = available | (static_cast<EntityT>(version) << entity_traits::entity_shift);
        available = entt;
    }


public:


    /**
    * @brief Checks if an entity identifier refers to a valid entity.
    * @param entity 
    *   An entity identifier, either valid or not.
    * @return True if the identifier is valid, false otherwise.
    */
    bool is_valid(const EntityT entity) const
    {
        const auto pos = static_cast<size_t>(entity & entity_traits::entity_mask);
        return (pos < entities.size() && entities[pos] == entity);
    }


    EntityT create_entity()
    {
        return available == null_entity ? generate_identifier() : recycle_identifier();
    }

    void destroy_entity(const EntityT entity)
    {
    }

    template<typename Component, typename... Args>
    Component& emplace(const EntityT entity, Args &&... args)
    {

    }



    template<typename... Components>
    void remove_components(const EntityT entity)
    {
        static_assert(sizeof...(Components) > 0, "Must provide one or more component types");

    }

    


private:
    std::vector<EntityT> entities;
    EntityT available{ null_entity };


};



}
}

#endif