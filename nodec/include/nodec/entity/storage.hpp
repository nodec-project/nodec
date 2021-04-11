#ifndef NODEC__ENTITY__STORAGE_HPP_
#define NODEC__ENTITY__STORAGE_HPP_

#include <nodec/entity/sparse_table.hpp>

#include <vector>

namespace nodec
{
namespace entity
{

class BaseStorage
{

};

template<typename Entity, typename Value>
class BasicStorage : public BaseStorage
{
public:
    using index_type = size_t;

    struct SparseData
    {
        size_t index;
        Entity entity;
    };
public:

    template<typename... Args>
    Value& emplace(const Entity entity, Args &&... args)
    {
        sparse_table[entity] = { instances.size(), entity };
        instances.emplace_back(std::forward<Args>(args)...);
        return instances.back();
    }

    void remove(const Entity entity)
    {
        auto& ref_to_remove = sparse_table[entity];
        instances[ref_to_remove.index] = std::move(instances.back());
        instances.pop_back();
        auto& other = sparse_table.back();
        other.index = ref_to_remove.index;
        sparse_table.erase(ref_to_remove.entity);
    }


private:
    SparseTable<SparseData> sparse_table;
    std::vector<Value> instances;
};


}
}



#endif