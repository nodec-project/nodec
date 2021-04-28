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
public:
    using Iterator = typename std::vector<Entity>::iterator;
    using ConstIterator = typename std::vector<Entity>::const_iterator;

public:
    virtual bool erase(const Entity entity) = 0;

public:
    virtual Iterator begin() noexcept = 0;
    virtual Iterator end() noexcept = 0;
    virtual ConstIterator cbegin() noexcept = 0;
    virtual ConstIterator cend() noexcept = 0;
    virtual size_t size() const noexcept = 0;

};


template<typename Entity, typename Value>
class BasicStorage : public BaseStorage<Entity>
{
    using Base = BaseStorage<Entity>;

public:
    template<typename... Args>
    std::pair<Value&, bool> emplace(const Entity entity, Args &&... args)
    {
        if (sparse_table.contains(entity))
        {
            return { instances[sparse_table[entity]], false };
        }

        sparse_table[entity] = instances.size();
        instances.push_back({ args... });
        packed.emplace_back(entity);
        return { instances.back(), true };
    }

    Value* try_get(const Entity entity)
    {
        if (!sparse_table.contains(entity))
        {
            return nullptr;
        }
        return &instances[sparse_table[entity]];
    }

    bool erase(const Entity entity)
    {
        if (!sparse_table.contains(entity))
        {
            return false;
        }

        // move the back of instances to the removed index.
        auto& pos = sparse_table[entity];

        auto& other_entity = packed.back();
        packed[pos] = other_entity;
        packed.pop_back();

        instances[pos] = std::move(instances.back());
        instances.pop_back();

        // link the location of the moved instance to the entity number.
        sparse_table[other_entity] = pos;
        sparse_table.erase(entity);

        return true;
    }

public:
    Base::Iterator begin() noexcept override
    {
        return packed.begin();
    }

    Base::Iterator end() noexcept override
    {
        return packed.end();
    }

    Base::ConstIterator cbegin() noexcept override
    {
        return packed.cbegin();
    }

    Base::ConstIterator cend() noexcept override
    {
        return packed.cend();
    }

    size_t size() const noexcept override
    {
        return packed.size();
    }

private:
    SparseTable<size_t> sparse_table;
    std::vector<Entity> packed;
    std::vector<Value> instances;
};


}
}



#endif