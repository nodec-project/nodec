#ifndef NODEC__RESOURCE_MANAGEMENT__RESOURCE_REGISTRY_HPP_
#define NODEC__RESOURCE_MANAGEMENT__RESOURCE_REGISTRY_HPP_

#include <nodec/type_info.hpp>
#include <nodec/error_formatter.hpp>

#include <type_traits>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <future>
#include <mutex>
#include <stdexcept>
#include <cassert>


namespace nodec {
namespace resource_management {

namespace details {

// Note-2021-10-03 <IOE>:
//  * I wonder about making an exception class for each exception content.
//  * An exception class should be created for each error type.

template<typename Type>
inline void throw_no_resource_exception(const std::string& resource_name, const char* file, size_t line) {
    throw std::runtime_error(nodec::error_fomatter::with_type_file_line<std::runtime_error>(
        nodec::Formatter()
        << "No resource named '" << resource_name << " (type: " << typeid(Type).name() << ")'",
        file, line));
}


template<typename Type>
inline void throw_resource_already_exists_exception(const std::string& resource_name, const char* file, size_t line) {
    throw std::runtime_error(nodec::error_fomatter::with_type_file_line<std::runtime_error>(
        nodec::Formatter()
        << "The resource named '" << resource_name << " (type: " << typeid(Type).name() << ")' already exisis.",
        file, line));
}

}



class ResourceRegistry {
    template<typename Type>
    using Resource = std::shared_ptr<Type>;

    template<typename Type>
    using ResourceSharedFuture = std::shared_future<std::shared_ptr<Type>>;

    template <typename Type>
    using ResourceFuture = std::future<std::shared_ptr<Type>>;

public:
    template<typename Type>
    class ResourceBlock;

    template<typename Type>
    class LoadBridge {
        using ResourceBlock = ResourceBlock<Type>;

    public:
        LoadBridge(ResourceBlock* block)
            : block_{ block } {
        }

        void on_resource_loaded(const std::string& name, Resource<Type> resource) const {
            {
                std::lock_guard<std::mutex> lock(block_->dict_mutex);
                block_->dict[name] = resource;
                block_->inverted_dict[reinterpret_cast<std::intptr_t>(resource.get())] = name;
            }

            {
                std::lock_guard<std::mutex> lock(block_->loading_futures_mutex);
                block_->loading_futures.erase(name);
            }
        }

    private:
        ResourceBlock* block_{ nullptr };
    };


private:

    struct BaseResourceBlock {
        virtual ~BaseResourceBlock() {};
    };

    template<typename Type>
    struct ResourceBlock : public BaseResourceBlock {

        using Loader = std::function<ResourceFuture<Type>(const std::string&, LoadBridge<Type>)>;

        ~ResourceBlock() {}

        std::unordered_map<std::string, std::weak_ptr<Type>> dict;
        std::unordered_map<std::string, ResourceSharedFuture<Type>> loading_futures;

        // ptr -> name
        std::unordered_map<std::intptr_t, std::string> inverted_dict;

        std::mutex dict_mutex;
        std::mutex loading_futures_mutex;

        Loader loader;
    };

    struct ResourceBlockData {
        std::unique_ptr<BaseResourceBlock> block;
    };


    template<typename Type>
    ResourceBlock<Type>* resource_block_assured() {
        std::lock_guard<std::mutex> lock(resource_blocks_mutex);

        const auto index = type_seq<Type>::value();

        if (!(index < resource_blocks.size())) {
            resource_blocks.resize(index + 1u);
        }

        auto&& blockData = resource_blocks[index];
        if (!blockData.block) {
            blockData.block.reset(new ResourceBlock<Type>());
        }

        return static_cast<ResourceBlock<Type>*>(blockData.block.get());
    }



public:

    ResourceRegistry() = default;

    ResourceRegistry(ResourceRegistry&&) = default;

    ResourceRegistry& operator=(ResourceRegistry&&) = default;


    /**
    *
    * @code{.cpp}
    * std::future<std::shared_ptr<Type>>(const std::string&, LoadBridge<Type>);
    * @endcode
    */
    template<typename Type, typename Loader>
    void register_resource_loader(Loader&& loader) {
        auto* block = resource_block_assured<Type>();

        block->loader = loader;
    }

    template<typename Type>
    ResourceSharedFuture<Type> get_resource(const std::string& name) {
        auto* block = resource_block_assured<Type>();

        Resource<Type> resource;

        {
            std::lock_guard<std::mutex> lock(block->dict_mutex);
            resource = block->dict[name].lock();
        }

        if (resource) {
            std::promise<Resource<Type>> promise;
            promise.set_value(resource);

            return promise.get_future().share();
        }

        bool is_loading = false;
        ResourceSharedFuture<Type>* future;

        {
            std::lock_guard<std::mutex> lock(block->loading_futures_mutex);
            is_loading = block->loading_futures.find(name) != block->loading_futures.end();
            future = &block->loading_futures[name];
        }

        if (is_loading) {
            assert(future->valid());
            return *future;
        }

        *future = block->loader(name, { block }).share();
        return *future;
    }

    template<typename Type>
    std::pair<std::string, bool> lookup_name(Resource<Type> resource) noexcept {
        auto* block = resource_block_assured<Type>();

        std::string name;

        // get resource pointer
        auto ptr = reinterpret_cast<std::intptr_t>(resource.get());
        {
            std::lock_guard<std::mutex> lock(block->dict_mutex);

            // lookup name from the pointer
            auto iter = block->inverted_dict.find(ptr);
            if (iter == block->inverted_dict.end()) {
                return { "", false };
            }

            auto booked = block->dict[iter->second].lock();
            if (!booked || resource != booked) {
                block->inverted_dict.erase(iter);
                return { "", false };
            }
            name = iter->second;
        }
        return { name, true };
    }

private:
    std::mutex resource_blocks_mutex;
    std::vector<ResourceBlockData> resource_blocks;
};


}
}


#endif