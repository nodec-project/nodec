#ifndef NODEC__ENTITIES__STORAGE_HPP_
#define NODEC__ENTITIES__STORAGE_HPP_

#include <nodec/entities/sparse_table.hpp>

#include <vector>

namespace nodec {
namespace entities {


template<typename Entity>
class BaseStorage {
public:
    using iterator = typename std::vector<Entity>::iterator;
    using const_iterator = typename std::vector<Entity>::const_iterator;

public:
    virtual bool contains(const Entity entity) const = 0;
    virtual bool erase(const Entity entity) = 0;
    virtual void* try_get_opaque(const Entity entity) = 0;

public:
    virtual iterator begin() noexcept = 0;
    virtual iterator end() noexcept = 0;
    virtual const_iterator begin() const noexcept = 0;
    virtual const_iterator end() const noexcept = 0;
    virtual const_iterator cbegin() const noexcept = 0;
    virtual const_iterator cend() const noexcept = 0;
    virtual size_t size() const noexcept = 0;

};


template<typename Entity, typename Value>
class BasicStorage : public BaseStorage<Entity> {
    static_assert(std::is_move_constructible_v<Value>&& std::is_move_assignable_v<Value>,
                  "The managed value must be at leat move constructible and move assignable.");

    using Base = BaseStorage<Entity>;

public:
    template<typename... Args>
    std::pair<Value&, bool> emplace(const Entity entity, Args &&... args) {
        if (sparse_table.contains(entity)) {
            return { instances[sparse_table[entity]], false };
        }

        sparse_table[entity] = instances.size();
        instances.push_back({ args... });
        packed.emplace_back(entity);
        return { instances.back(), true };
    }

    const Value* try_get(const Entity entity) const {
        auto* pos = sparse_table.try_get(entity);
        if (!pos) {
            return nullptr;
        }

        return &instances[*pos];
    }

    Value* try_get(const Entity entity) {
        auto* pos = sparse_table.try_get(entity);
        if (!pos) {
            return nullptr;
        }

        return &instances[*pos];
    }

    void* try_get_opaque(const Entity entity) override {
        return try_get(entity);
    }

    bool contains(const Entity entity) const override {
        return sparse_table.contains(entity);
    }

    bool erase(const Entity entity) {
        auto* pos = sparse_table.try_get(entity);
        if (!pos) {
            return false;
        }

        // move the back of instances to the removed index.
        auto& other_entity = packed.back();
        packed[*pos] = other_entity;
        packed.pop_back();

        instances[*pos] = std::move(instances.back());
        instances.pop_back();

        // link the location of the moved instance to the entity number.
        sparse_table[other_entity] = *pos;
        sparse_table.erase(entity);

        return true;
    }

public:
    Base::iterator begin() noexcept override {
        return packed.begin();
    }

    Base::iterator end() noexcept override {
        return packed.end();
    }

    Base::const_iterator begin() const noexcept override {
        return packed.begin();
    }

    Base::const_iterator end() const noexcept override {
        return packed.end();
    }

    Base::const_iterator cbegin() const noexcept override {
        return packed.cbegin();
    }

    Base::const_iterator cend() const noexcept override {
        return packed.cend();
    }

    size_t size() const noexcept override {
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