#ifndef NODEC__ENTITY__REGISTRY_HPP_
#define NODEC__ENTITY__REGISTRY_HPP_

#include <nodec/entity/entity.hpp>

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

public:
    BasicRegistry() = default;
    BasicRegistry(BasicRegistry&&) = default;

    /**
    * @brief Default move assignment operator.
    */
    BasicRegistry& operator=(BasicRegistry&&) = default;


    EntityT create_entity()
    {

    }

    void destroy_entity(const EntityT entity)
    {

    }


    template<typename... Components>
    void remove_components(const EntityT entity)
    {
        static_assert(sizeof...(Components) > 0, "Must provide one or more component types");

    }

private:
    EntityT generate_identifier()
    {
        return entities.emplace_back(static_cast<EntityT>(entities.size()));
    }

    EntityT recycle_identifier()
    {

    }

    void release_entity(const EntityT entity)
    {

    }

private:
    std::vector<EntityT> entities{};


};



}
}

#endif