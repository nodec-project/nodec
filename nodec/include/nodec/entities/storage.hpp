#ifndef NODEC__ENTITIES__STORAGE_HPP_
#define NODEC__ENTITIES__STORAGE_HPP_

#include "../containers/sparse_table.hpp"
#include "../formatter.hpp"
#include "../signals/signal.hpp"
#include "../type_traits.hpp"

#include <cassert>
#include <iterator>
#include <vector>

namespace nodec {
namespace entities {

namespace internal {

/**
 * @brief Internal storage iterator.
 *
 * This iterator walks the packed entity storage in reverse order.
 * This is useful for deleting the element while iterating.
 *
 * @tparam Container
 */
template<typename Container>
class StorageIterator final {
public:
    using value_type = typename Container::value_type;
    using pointer = typename Container::const_pointer;
    using reference = typename Container::const_reference;
    using difference_type = typename Container::difference_type;
    using iterator_type = std::random_access_iterator_tag;

    constexpr StorageIterator() noexcept
        : packed_{}, offset_{} {}

    constexpr StorageIterator(const Container &ref, const difference_type offset) noexcept
        : packed_{std::addressof(ref)}, offset_{offset} {}

    constexpr StorageIterator &operator++() noexcept {
        return --offset_, *this;
    }

    constexpr StorageIterator operator++(int) noexcept {
        StorageIterator orig = *this;
        return ++(*this), orig;
    }

    constexpr StorageIterator &operator--() noexcept {
        return ++offset_, *this;
    }

    constexpr StorageIterator operator--(int) noexcept {
        StorageIterator orig = *this;
        return --(*this), orig;
    }

    constexpr StorageIterator &operator+=(const difference_type value) noexcept {
        offset_ -= value;
        return *this;
    }

    constexpr StorageIterator operator+(const difference_type value) const noexcept {
        StorageIterator copy = *this;
        return (copy += value);
    }

    constexpr StorageIterator &operator-=(const difference_type value) noexcept {
        return (*this += -value);
    }

    constexpr StorageIterator operator-(const difference_type value) const noexcept {
        return (*this + -value);
    }

    constexpr pointer operator->() const noexcept {
        return packed_->data() + index();
    }

    constexpr reference operator*() const noexcept {
        return *operator->();
    }

    constexpr difference_type index() const noexcept {
        return offset_ - 1;
    }

private:
    const Container *packed_;
    difference_type offset_;
};

template<typename Container>
constexpr std::ptrdiff_t operator-(const StorageIterator<Container> &lhs, const StorageIterator<Container> &rhs) noexcept {
    return rhs.index() - lhs.index();
}

template<typename Container>
constexpr bool operator==(const StorageIterator<Container> &lhs, const StorageIterator<Container> &rhs) noexcept {
    return lhs.index() == rhs.index();
}

template<typename Container>
constexpr bool operator!=(const StorageIterator<Container> &lhs, const StorageIterator<Container> &rhs) noexcept {
    return !(lhs == rhs);
}

template<typename Container>
constexpr bool operator<(const StorageIterator<Container> &lhs, const StorageIterator<Container> &rhs) noexcept {
    // The iterator iterates over the storage in reverse order.
    return lhs.index() > rhs.index();
}

template<typename Container>
constexpr bool operator>(const StorageIterator<Container> &lhs, const StorageIterator<Container> &rhs) noexcept {
    // The iterator iterates over the storage in reverse order.
    return lhs.index() < rhs.index();
}

template<typename Container>
constexpr bool operator<=(const StorageIterator<Container> &lhs, const StorageIterator<Container> &rhs) noexcept {
    return !(lhs > rhs);
}

template<typename Container>
constexpr bool operator>=(const StorageIterator<Container> &lhs, const StorageIterator<Container> &rhs) noexcept {
    return !(lhs < rhs);
}

} // namespace internal

template<typename Entity>
class BasicRegistry;

template<typename Entity>
class BaseStorage {
    using PackedContainer = std::vector<Entity>;
    using SparseTable = containers::SparseTable<size_t>;

public:
    using entity_type = Entity;

    using const_iterator = internal::StorageIterator<PackedContainer>;
    using iterator = const_iterator;

public:
    BaseStorage(PackedContainer &packed, SparseTable &sparse_table)
        : packed_{packed}, sparse_table_{sparse_table} {}

    virtual ~BaseStorage() {}

public:
    void bind_registry(BasicRegistry<Entity> *registry) {
        registry_ = registry;
    }

    BasicRegistry<Entity> *registry() const noexcept {
        return registry_;
    }

    const_iterator begin() const noexcept {
        const auto pos = static_cast<typename iterator::difference_type>(packed_.size());
        return iterator{packed_, pos};
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator end() const noexcept {
        return iterator{packed_, {}};
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

    virtual bool erase(const Entity entity) = 0;
    virtual void clear() = 0;
    virtual void *try_get_opaque(const Entity entity) = 0;

    template<typename It>
    std::size_t erase(It first, It last) {
        std::size_t count{};

        for (; first != last; ++first) {
            if (erase(*first)) ++count;
        }
        return count;
    }

private:
    //! Compressed entity vector.
    PackedContainer &packed_;

    //! Conversion table from entity to packed index.
    SparseTable &sparse_table_;

    BasicRegistry<Entity> *registry_;
};

template<typename Entity, typename Value>
class BasicStorage : public BaseStorage<Entity> {
    static_assert(std::is_move_constructible<Value>::value && std::is_move_assignable<Value>::value,
                  "The managed value must be at least move constructible and move assignable.");

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
    std::pair<Value &, bool> emplace(const Entity entity, Args &&...args) {
        {
            auto *pos = sparse_table.try_get(entity);
            if (pos != nullptr) return {instances[*pos], false};
        }

        auto pos = instances.size();
        sparse_table[entity] = pos;
        instances.push_back({args...});
        packed.emplace_back(entity);

        element_constructed_(*this->registry(), entity);

        return {instances[pos], true};
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

    const value_type &get(const Entity entity) const {
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

    bool erase(const Entity entity) override {
        if (!sparse_table.contains(entity)) {
            return false;
        }

        element_destroyed_(*this->registry(), entity); // cause structural changes.

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

    using base_type::erase;

    void clear() override {
        while (packed.size() > 0) {
            erase(packed.front());
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
template<typename Entity, typename Type>
struct storage_for {
    using type = constness_as_t<BasicStorage<Entity, std::remove_const_t<Type>>, Type>;
};

template<typename Entity, typename Type>
using storage_for_t = typename storage_for<Entity, Type>::type;

} // namespace entities
} // namespace nodec

#endif