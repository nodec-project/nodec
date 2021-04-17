#ifndef NODEC__ENTITY__STORAGE_HPP_
#define NODEC__ENTITY__STORAGE_HPP_

#include <nodec/ecs/sparse_table.hpp>

#include <vector>

namespace nodec
{
namespace ecs
{

template<typename Entity>
class BaseStorage
{

};

template<typename Entity, typename Value>
class BasicStorage : public BaseStorage<Entity>
{
public:
    struct Container
    {
        Entity entity;
        Value value;

        template<typename... Args>
        Container(Entity entity, Args&& ...args)
            :entity(entity), value{ args... }
        {
        }
    };
public:

    template<typename... Args>
    std::pair<Value&, bool> emplace(const Entity entity, Args &&... args)
    {
        if (sparse_table.contains(entity))
        {
            return { instances[sparse_table[entity]].value, false };
        }

        sparse_table[entity] = instances.size();
        //instances.push_back({ entity, args... });
        instances.emplace_back(entity, std::forward<Args>(args)... );
        return { instances.back().value, true };
    }

    Value* try_get(const Entity entity)
    {
        if (!sparse_table.contains(entity))
        {
            return nullptr;
        }
        return &instances[sparse_table[entity]].value;
    }

    void remove(const Entity entity)
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