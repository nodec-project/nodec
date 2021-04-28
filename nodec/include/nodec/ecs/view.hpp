#ifndef NODEC__ECS__VIEW_HPP_
#define NODEC__ECS__VIEW_HPP_

#include <nodec/ecs/storage.hpp>

#include <nodec/fold.hpp>

#include <array>

namespace nodec
{
namespace ecs
{

template<typename It>
class ViewIterator final
{

public:


private:
    It first;
    It last;
    It it;


};

template<typename Entity, typename... Components>
class BasicView final
{
    template<typename Comp>
    using Storage = BasicStorage<Entity, Comp>;

    using OtherStorages = std::array<const BaseStorage<Entity>*, (sizeof...(Components) - 1)>;

    BaseStorage<Entity>* base_storage_candidate() const noexcept
    {
        return (std::min)({ static_cast<BaseStorage<Entity>*>(std::get<Storage<Components>*>(pools))... }, [](const auto* lhs, const auto* rhs)
                          {
                              return lhs->size() < rhs->size();
                          });
    }

    OtherStorages get_other_storages(const BaseStorage<Entity>* cpool) const noexcept
    {
        size_t pos{};
        OtherStorages others;

        using Expander = int[];

        (void)Expander
        {
            [&](auto* pool)
            {
                logging::InfoStream(__FILE__, __LINE__) << pool;
                return (pool == cpool ? 0 : (others[pos++] = pool, 0));
                //return static_cast<void>(pool == cpool ? void() : void(others[pos++] = pool));
            }(std::get<Storage<Components>*>(pools))...
        };


        //fold([&](auto* pool)
        //     {
        //         return static_cast<void>(pool == cpool ? void() : void(others[pos++] = pool));
        //     },
        //     std::get<Storage<Components>*>(pools)...);

        return others;
    }

public:
    using Iterator = ViewIterator<typename BaseStorage<Entity>::Iterator>;

    BasicView(Storage<Components>&... components)
        : pools{ &components... },
        base_storage{ base_storage_candidate() }
    {
    }

    Iterator begin() noexcept
    {

    }

    void test()
    {
        auto other_storages = get_other_storages(base_storage);

        logging::InfoStream(__FILE__, __LINE__) << base_storage  << ", " << other_storages[0];
    }


private:
    std::tuple<Storage<Components>*...> pools;
    BaseStorage<Entity>* base_storage;
};

}
}

#endif