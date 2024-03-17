#ifndef NODEC__ENTITIES__VIEW_HPP_
#define NODEC__ENTITIES__VIEW_HPP_

#include "exceptions.hpp"
#include "storage.hpp"

#include <algorithm>
#include <array>
#include <iterator>

namespace nodec {
namespace entities {

namespace internal {

template<typename Type, std::size_t StorageCount, std::size_t ExclusionCount>
class ViewIterator final {
    using Iterator = typename Type::const_iterator;

    bool valid() const noexcept {
        const auto entt = *it;
        return ((StorageCount != 0) || (entt != tombstone_entity))
               && std::all_of(pools.cbegin(), pools.cend(), [&](const auto *curr) { return curr->contains(entt); })
               && std::none_of(filter.cbegin(), filter.cend(), [&](const auto *curr) { return curr->contains(entt); });
    }

public:
    // For stl-iterator compatible.
    using value_type = typename Iterator::value_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
    using difference_type = typename Iterator::difference_type;
    using iterator_category = std::forward_iterator_tag;

    constexpr ViewIterator() noexcept
        : it{}, last{}, pools{}, filter{} {}

    ViewIterator(Iterator curr, Iterator to, std::array<const Type *, StorageCount> all_of, std::array<const Type *, ExclusionCount> none_of) noexcept
        : it{curr}, last{to}, pools{all_of}, filter{none_of} {
        while (it != last && !valid()) ++it;
    }

    ViewIterator &operator++() noexcept {
        while (++it != last && !valid()) {}
        return *this;
    }

    ViewIterator operator++(int) noexcept {
        ViewIterator orig = *this;
        return ++(*this), orig;
    }

    pointer operator->() const noexcept {
        return &*it;
    }

    reference operator*() const noexcept {
        return *operator->();
    }

    bool operator==(const ViewIterator &other) const noexcept {
        return other.it == it;
    }

    bool operator!=(const ViewIterator &other) const noexcept {
        return !(*this == other);
    }

private:
    Iterator it;
    Iterator last;
    std::array<const Type *, StorageCount> pools;
    std::array<const Type *, ExclusionCount> filter;
};

} // namespace internal

template<typename, typename>
class BasicView;

/**
 *
 */
template<typename... Storages, typename... Exclusions>
class BasicView<type_list<Storages...>, type_list<Exclusions...>> {
    using Entity = std::common_type_t<typename Storages::entity_type..., typename Exclusions::entity_type...>;
    using BaseStorage = std::common_type_t<typename Storages::base_type..., typename Exclusions::base_type...>;

    template<typename Type>
    static constexpr std::size_t index_of = type_list_index_v<std::remove_const_t<Type>, type_list<typename Storages::value_type...>>;

    // template<std::size_t I>
    // struct component_at {
    //     using type = constness_as_t<type_list_element_t<I, type_list<typename Storages::value_type...>>,
    //                                 type_list_element_t<I, type_list<Storages...>>> &;
    // };

    // template<std::size_t I>
    // using component_at_t = typename component_at<I>::type;

    auto base_storage_candidate() const noexcept {
        return (std::min)({static_cast<const BaseStorage *>(std::get<Storages *>(pools))...},
                          [](const auto *lhs, const auto *rhs) {
                              return lhs->size() < rhs->size();
                          });
    }

    auto opaque_check_set() const noexcept {
        std::array<const BaseStorage *, sizeof...(Storages) - 1u> others{};
        size_t pos{};

        using Expander = int[];

        (void)Expander{
            [&](const auto *curr) {
                return (curr == base_storage ? 0 : (others[pos++] = curr, 0));
            }(std::get<Storages *>(pools))...};
        return others;
    }

    auto filter_as_array() const noexcept {
        return std::array<const BaseStorage *, sizeof...(Exclusions)>{
            std::get<Exclusions *>(filter)...};
    }

public:
    // like-stl.
    using iterator = internal::ViewIterator<BaseStorage, sizeof...(Storages) - 1u, sizeof...(Exclusions)>;

    BasicView(Storages &...storages, Exclusions &...exclusions) noexcept
        : pools{&storages...},
          filter{&exclusions...},
          base_storage{base_storage_candidate()} {
    }

    iterator begin() const noexcept {
        return {base_storage->begin(), base_storage->end(), opaque_check_set(), filter_as_array()};
    }

    iterator end() const noexcept {
        return {base_storage->end(), base_storage->end(), opaque_check_set(), filter_as_array()};
    }

    decltype(auto) get(const Entity entt) const {
        return std::tuple_cat([&](auto *pool) { return pool->get_as_tuple(entt); }(std::get<Storages *>(pools))...);
    }

    template<std::size_t I>
    decltype(auto) get(const Entity entt) const {
        return std::get<I>(pools)->get(entt);
    }

    template<typename Comp>
    decltype(auto) get(const Entity entt) const {
        return get<index_of<Comp>>(entt);
    }

    Entity front() const noexcept {
        const auto it = begin();
        return it != end() ? *it : null_entity;
    }

    /**
     * @brief
     *
     * @tparam Func
     * @param func void(SceneEntity, Components...)
     */
    template<typename Func>
    void each(Func func) const {
        for (const auto entt : *this) {
            func(entt, [&](auto *pool) -> decltype(auto) { return pool->get(entt); }(std::get<Storages *>(pools))...);
        }
    }

private:
    std::tuple<Storages *...> pools;
    std::tuple<Exclusions *...> filter;
    const BaseStorage *base_storage;
};

} // namespace entities
} // namespace nodec

#endif