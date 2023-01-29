#ifndef NODEC__ENTITIES__STORAGE_HPP_
#define NODEC__ENTITIES__STORAGE_HPP_

#include "../containers/sparse_table.hpp"
#include "../formatter.hpp"
#include "../signals/signal.hpp"
#include "../type_traits.hpp"
#include "../utility.hpp"
#include "entity.hpp"

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
    using entity_traits_type = entity_traits<Entity>;

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

    /**
     * @brief Checks if a storage contains an entity.
     */
    bool contains(const Entity entity) const {
        const auto entity_number = entity_traits_type::to_entity(entity);
        auto *pos = sparse_table_.try_get(entity_number);
        if (pos == nullptr) return false;

        return packed_[*pos] == entity;
    }

    virtual bool erase(const Entity entity) = 0;
    virtual void *try_get_opaque(const Entity entity) = 0;

    template<typename It>
    std::size_t erase(It first, It last) {
        std::size_t count{};

        for (; first != last; ++first) {
            if (erase(*first)) ++count;
        }
        return count;
    }
    
    virtual void clear() = 0;

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

public:
    // like stl.
    using value_type = Value;
    using entity_type = Entity;
    using base_type = Base;

public:
    BasicStorage()
        : Base{packed_, sparse_table_} {}

    /**
     * @brief Assigns an entity to a storage and constructs its object.
     *
     * If the entity is already exists, return the reference to its object.
     *
     * @warning
     * The version of the given entity that already exists must match the one in storage.
     */
    template<typename... Args>
    std::pair<value_type &, bool> emplace(const Entity entity, Args &&...args) {
        const auto entity_number = Base::entity_traits_type::to_entity(entity);

        {
            auto *pos = sparse_table_.try_get(entity_number);
            if (pos != nullptr) {
                assert(packed_[*pos] == entity);
                return {instances_[*pos], false};
            }
        }

        auto pos = instances_.size();
        sparse_table_[entity_number] = pos;
        instances_.push_back({args...});
        packed_.emplace_back(entity);

        element_constructed_(*this->registry(), entity);

        return {instances_[pos], true};
    }

    const value_type *try_get(const Entity entity) const {
        const auto *pos = sparse_table_.try_get(Base::entity_traits_type::to_entity(entity));
        if (!pos) return nullptr;

        assert(packed_[*pos] == entity);

        return &instances_[*pos];
    }

    value_type *try_get(const Entity entity) {
        return const_cast<value_type *>(as_const(*this).try_get(entity));
    }

    const value_type &get(const Entity entity) const {
        const auto *pos = sparse_table_.try_get(Base::entity_traits_type::to_entity(entity));
        assert(pos != nullptr);
        assert(packed_[*pos] == entity);

        return instances_[*pos];
    }

    value_type &get(const Entity entity) {
        return const_cast<value_type &>(as_const(*this).get(entity));
    }

    std::tuple<const value_type &> get_as_tuple(const Entity entity) const {
        return std::forward_as_tuple(get(entity));
    }

    std::tuple<value_type &> get_as_tuple(const Entity entity) {
        return std::forward_as_tuple(get(entity));
    }

public:
    // --- override functions ---

    void *try_get_opaque(const Entity entity) override {
        return try_get(entity);
    }

    bool erase(const Entity entity) override {
        if (!contains(entity)) return false;

        element_destroyed_(*this->registry(), entity); // cause structural changes.

        const auto entity_number = Base::entity_traits_type::to_entity(entity);
        auto *pos = sparse_table_.try_get(entity_number);
        assert(pos && "The entity to be deleted has already been deleted. Have you deleted the same entity again in the destroy signal?");

        // move the back of instances to the removed index.
        auto &other_entity = packed_.back();
        packed_[*pos] = other_entity;
        packed_.pop_back();

        instances_[*pos] = std::move(instances_.back());
        instances_.pop_back();

        // Updates the location of other entity.
        sparse_table_[Base::entity_traits_type::to_entity(other_entity)] = *pos;
        sparse_table_.erase(entity_number);

        return true;
    }

    using base_type::erase;

    void clear() override {
        while (packed_.size() > 0) {
            erase(packed_.front());
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
    containers::SparseTable<size_t> sparse_table_;
    std::vector<Entity> packed_;
    std::vector<Value> instances_;

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