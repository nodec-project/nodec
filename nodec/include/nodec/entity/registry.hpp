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

    }

    void release_entity(const Entity entity)
    {

    }


public:
    BasicRegistry() = default;
    BasicRegistry(BasicRegistry&&) = default;

    /**
    * @brief Default move assignment operator.
    */
    BasicRegistry& operator=(BasicRegistry&&) = default;


    Entity create_entity()
    {

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


};



}
}

#endif