#ifndef NODEC__ENTITIES__VIEW_HPP_
#define NODEC__ENTITIES__VIEW_HPP_

#include <nodec/entities/storage.hpp>

#include <algorithm>
#include <array>

namespace nodec {
namespace entities {

template<typename Entity, typename... Components>
class BasicView final {
    template<typename Comp>
    using Storage = BasicStorage<Entity, std::remove_const_t<Comp>>;

    using UncheckedStorages = std::array<const BaseStorage<Entity> *, (sizeof...(Components) - 1)>;

    template<typename It>
    class ViewIterator final {
        bool valid() const {
            const auto entt = *it;
            return std::all_of(unchecked_storages.cbegin(), unchecked_storages.cend(),
                               [&](const BaseStorage<Entity> *curr) {
                                   return curr->contains(entt);
                               });
        }

    public:
        using pointer = typename std::iterator_traits<It>::pointer;
        using reference = typename std::iterator_traits<It>::reference;

        ViewIterator(It from, It to, It curr, UncheckedStorages others) noexcept
            : first{from},
              last{to},
              it{curr},
              unchecked_storages{others} {
            if (it != last && !valid()) {
                ++(*this);
            }
        }

        ViewIterator &operator++() noexcept {
            while (++it != last && !valid())
                ;
            return *this;
        }

        ViewIterator operator++(int) noexcept {
            ViewIterator orig = *this;
            return ++(*this), orig;
        }

        bool operator==(const ViewIterator &other) const noexcept {
            return other.it == it;
        }

        bool operator!=(const ViewIterator &other) const noexcept {
            return !(*this == other);
        }

        pointer operator->() const {
            return &*it;
        }

        reference operator*() const {
            return *operator->();
        }

    private:
        It first;
        It last;
        It it;
        UncheckedStorages unchecked_storages;
    };

    BaseStorage<Entity> *base_storage_candidate() const noexcept {
        return (std::min)({static_cast<BaseStorage<Entity> *>(std::get<Storage<Components> *>(pools))...},
                          [](const auto *lhs, const auto *rhs) {
                              return lhs->size() < rhs->size();
                          });
    }

    UncheckedStorages get_unchecked_storages(const BaseStorage<Entity> *cpool) const noexcept {
        size_t pos{};
        UncheckedStorages others;

        using Expander = int[];

        (void)Expander{
            [&](auto *pool) {
                return (pool == cpool ? 0 : (others[pos++] = pool, 0));
            }(std::get<Storage<Components> *>(pools))...};

        return others;
    }

public:
    using iterator = ViewIterator<typename BaseStorage<Entity>::iterator>;

    BasicView(Storage<Components> &...components)
        : pools{&components...},
          base_storage{base_storage_candidate()} {
    }

    iterator begin() const noexcept {
        return {base_storage->begin(), base_storage->end(), base_storage->begin(), get_unchecked_storages(base_storage)};
    }

    iterator end() const noexcept {
        return {base_storage->begin(), base_storage->end(), base_storage->end(), get_unchecked_storages(base_storage)};
    }

private:
    std::tuple<Storage<Components> *...> pools;
    BaseStorage<Entity> *base_storage;
};

} // namespace entities
} // namespace nodec

#endif