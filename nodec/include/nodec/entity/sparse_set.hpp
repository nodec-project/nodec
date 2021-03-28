#ifndef NODEC__ENTITY__SPARSE_SET_HPP_
#define NODEC__ENTITY__SPARSE_SET_HPP_

#include <nodec/entity/entity.hpp>

#include <vector>
#include <memory>

namespace nodec
{
namespace entity
{

template<typename Entity>
class BasicSparseSet
{
private:
    static constexpr auto page_size = 4096;

    using entity_traits = entity_traits<Entity>;
    using Page = std::unique_ptr<Entity[]>;


    size_t page(const Entity entity) const noexcept
    {
        return static_cast<size_t>((entity & entity_traits::entity_mask) / page_size);
    }

    size_t offset(const Entity entity) const noexcept
    {
        return static_cast<size_t>(entity & (page_size - 1));
    }

    Page& assured_page(const size_t pos)
    {
        if (!(pos < sparse.size()))
        {
            sparse.resize(pos + 1);
        }

        if (!sparse[pos])
        {
            sparse[pos].reset(new Entity[page_size]);

            for (auto* first = sparse[pos].get(), *last = first + page_size; first != last; ++first)
            {
                *first = null_entity;
            }
        }

        return sparse[pos];
    }



public:

    void shrink_to_fit()
    {
        if (packed.empty())
        {
            sparse.clear();
        }

        sparse.shrink_to_fit();
        packed.shrink_to_fit();
    }

    /**
    * @brief Assigns an entity to a sparse set.
    */
    void emplace(const Entity entity)
    {
        assured_page(page(entity))[offset(entity)] = static_cast<Entity>(packed.size());
        packed.push_back(entity);
    }


    /**
    * @brief Removes an entity from a sparse set.
    */
    void remove(const Entity entity)
    {
        auto& ref = sparse[page(entity)][offset(entity)];

        const auto other = packed.back();
        sparse[page(other)][offset(other)] = ref;
        packed[static_cast<size_t>(ref)] = other;
        ref = null_entity;

        packed.pop_back();
    }

private:
    std::vector<Page> sparse;
    std::vector<Entity> packed;
};


}
}
#endif