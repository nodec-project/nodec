#ifndef NODEC__ENTITY__STORAGE_HPP_
#define NODEC__ENTITY__STORAGE_HPP_

#include <nodec/entity/sparse_table.hpp>

#include <vector>

namespace nodec
{
namespace entity
{

template<typename EntityT>
class BaseStorage
{

};

template<typename EntityT, typename ValueT>
class BasicStorage : public BaseStorage<EntityT>
{
public:
    struct Container
    {
        EntityT entity;
        ValueT value;
    };
public:

    template<typename... Args>
    ValueT& emplace(const EntityT entity, Args &&... args)
    {
        sparse_table[entity] = { instances.size(), entity };
        instances.emplace_back(entity, std::forward<Args>(args)...);
        return instances.back().value;
    }

    void remove(const EntityT entity)
    {
        if (!sparse_table.contains(entity))
        {
            return;
        }

        // move the back of instances to the removed index.
        auto& index = sparse_table[entity];
        instances[index] = std::move(instances.back());
        instances.pop_back();

        // link the location of the moved instance to the entity number.
        sparse_table[instances[index].entity] = index;
        sparse_table.erase(entity);
    }


private:
    SparseTable<size_t> sparse_table;
    std::vector<Container> instances;
};


}
}



#endif