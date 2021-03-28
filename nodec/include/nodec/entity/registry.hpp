#ifndef NODEC__ENTITY__REGISTRY_HPP_
#define NODEC__ENTITY__REGISTRY_HPP_

#include <nodec/entity/entity.hpp>
#include <nodec/entity/sparse_set.hpp>

#include <memory>
#include <vector>


namespace nodec
{
namespace entity
{

template<typename Entity>
class BasicRegistry
{
private:
    using entity_traits = entity_traits<Entity>;

    struct PoolData
    {
        std::unique_ptr<BasicSparseSet<Entity>> pool;
    };

    template<typename Component>


    Entity generate_identifier()
    {
        return entities.emplace_back(static_cast<Entity>(entities.size()));
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


public:


    Entity create_entity()
    {
        return available == null_entity ? generate_identifier() : recycle_identifier();
    }

    void destroy_entity(const Entity entity)
    {
    }




    template<typename... Components>
    void remove_components(const Entity entity)
    {
        static_assert(sizeof...(Components) > 0, "Must provide one or more component types");

    }

    


private:
    std::vector<Entity> entities;
    Entity available{ null_entity };


};



}
}

#endif