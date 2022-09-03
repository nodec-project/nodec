#ifndef NODEC__ENTITIES__STORAGE_HPP_
#define NODEC__ENTITIES__STORAGE_HPP_

#include "../signals.hpp"
#include "../type_traits.hpp"
#include "../containers/sparse_table.hpp"

#include <cassert>
#include <vector>

namespace nodec {
namespace entities {

template<typename Entity>
class BasicRegistry;

template<typename EntityT>
class BaseStorage {
    using PackedContainer = std::vector<EntityT>;
    using SparseTable = containers::SparseTable<size_t>;

public:
    using Entity = EntityT;

    using const_iterator = typename PackedContainer::const_iterator;

public:
    BaseStorage(PackedContainer &packed, SparseTable &sparse_table)
        : packed_{packed}, sparse_table_{sparse_table} {}

    virtual ~BaseStorage() {}

    // TODO: Support move semantics.

public:
    const_iterator begin() const noexcept {
        return packed_.begin();
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator end() const noexcept {
        return packed_.end();
    }
    const_iterator cend() const noexcept {
        return end();
    }

    size_t size() const noexcept {
        return packed_.size();
    }

    bool contains(const Entity entity) const {
        return sparse_table_.contains(entity);
    }

    virtual bool erase(BasicRegistry<Entity> &owner, const Entity entity) = 0;
    virtual void clear(BasicRegistry<Entity> &owner) = 0;
    virtual void *try_get_opaque(const Entity entity) = 0;

private:
    PackedContainer &packed_;
    SparseTable &sparse_table_;
};

template<typename Value, typename Entity>
class BasicStorage : public BaseStorage<Entity> {
    static_assert(std::is_move_constructible_v<Value> && std::is_move_assignable_v<Value>,
                  "The managed value must be at leat move constructible and move assignable.");

    using StorageSignal = signals::Signal<void(BasicRegistry<Entity> &, const Entity)>;

    using Base = BaseStorage<Entity>;

    void throw_out_of_range_exception(Entity entity, const char *file, const std::size_t line) const {
        throw std::out_of_range(ErrorFormatter<std::out_of_range>(file, line)
                                << "Storage {" << typeid(BasicStorage).name() << "} does not contains the entity {0x"
                                << std::hex << entity << "; entity: 0x" << to_entity(entity) << "; version: 0x" << to_version(entity) << "}.");
    }

public:
    // like stl.
    using value_type = Value;
    using entity_type = Entity;
    using base_type = Base;

public:
    BasicStorage()
        : Base{packed, sparse_table} {}

    template<typename... Args>
    bool emplace(BasicRegistry<Entity> &owner, const Entity entity, Args &&...args) {
        if (sparse_table.contains(entity)) return false;

        sparse_table[entity] = instances.size();
        instances.push_back({args...});
        packed.emplace_back(entity);

        element_constructed_(owner, entity);

        return true;
    }

    const Value *try_get(const Entity entity) const {
        auto *pos = sparse_table.try_get(entity);
        if (!pos) return nullptr;

        return &instances[*pos];
    }

    Value *try_get(const Entity entity) {
        auto *pos = sparse_table.try_get(entity);
        if (!pos) return nullptr;

        return &instances[*pos];
    }

    const value_type& get(const Entity entity) const {
        auto *pos = sparse_table.try_get(entity);
        if (!pos) throw_out_of_range_exception(entity, __FILE__, __LINE__);

        return instances[*pos];
    }

    value_type &get(const Entity entity) {
        auto *pos = sparse_table.try_get(entity);
        if (!pos) throw_out_of_range_exception(entity, __FILE__, __LINE__);

        return instances[*pos];
    }

    std::tuple<const value_type &> get_as_tuple(const entity_type entity) const {
        auto *pos = sparse_table.try_get(entity);
        if (!pos) throw_out_of_range_exception(entity, __FILE__, __LINE__);

        return std::forward_as_tuple(instances[*pos]);
    }

    std::tuple<value_type &> get_as_tuple(const entity_type entity) {
        auto *pos = sparse_table.try_get(entity);
        if (!pos) throw_out_of_range_exception(entity, __FILE__, __LINE__);

        return std::forward_as_tuple(instances[*pos]);
    }

public:
    // --- override functions ---

    void *try_get_opaque(const Entity entity) override {
        return try_get(entity);
    }

    bool erase(BasicRegistry<Entity> &owner, const Entity entity) override {
        if (!sparse_table.contains(entity)) {
            return false;
        }

        element_destroyed_(owner, entity); // cause structual changes.

        auto *pos = sparse_table.try_get(entity);
        assert(pos && "The entity to be deleted has already been deleted. Have you deleted the same entity again in the destroy signal?");

        // move the back of instances to the removed index.
        auto &other_entity = packed.back();
        packed[*pos] = other_entity;
        packed.pop_back();

        instances[*pos] = std::move(instances.back());
        instances.pop_back();

        // link the location of the moved instance to the entity number.
        sparse_table[other_entity] = *pos;
        sparse_table.erase(entity);

        return true;
    }

    void clear(BasicRegistry<Entity> &owner) override {
        while (packed.size() > 0) {
            erase(owner, packed.front());
        }
    }

public:
    decltype(auto) element_constructed() {
        return element_constructed_.signal_interface();
    }

    decltype(auto) element_destroyed() {
        return element_destroyed_.signal_interface();
    }

private:
    containers::SparseTable<size_t> sparse_table;
    std::vector<Entity> packed;
    std::vector<Value> instances;

    StorageSignal element_constructed_;
    StorageSignal element_destroyed_;
};

/**
 * @brief Type-to-storage conversion utility that preserves constness.
 */
template<typename Type, typename Entity>
struct storage_for {
    using type = constness_as_t<BasicStorage<std::remove_const_t<Type>, Entity>, Type>;
};

template<typename Type, typename Entity>
using storage_for_t = typename storage_for<Type, Entity>::type;

} // namespace entities
} // namespace nodec

#endif