#ifndef NODEC__RESOURCE_MANAGEMENT__RESOURCE_REGISTRY_HPP_
#define NODEC__RESOURCE_MANAGEMENT__RESOURCE_REGISTRY_HPP_

#include <nodec/flags/flags.hpp>
#include <nodec/formatter.hpp>
#include <nodec/type_info.hpp>

#include <cassert>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace nodec {
namespace resource_management {

/**
 * @brief Specifies the load policy for a task executed by the resource loader.
 */
enum class LoadPolicy {
    //! The task is executed on a different thread.
    Async,

    //! The task is executed on the calling thread and blocks the process.
    Direct
};

namespace details {

// Note-2021-10-03 <IOE>:
//  * I wonder about making an exception class for each exception content.
//  * An exception class should be created for each error type.

template<typename Type>
inline void throw_no_resource_exception(const std::string &resource_name, const char *file, size_t line) {
    throw std::runtime_error(ErrorFormatter<std::runtime_error>(file, line)
                             << "No resource named '" << resource_name << " (type: " << typeid(Type).name() << ")'");
}

template<typename Type>
inline void throw_resource_already_exists_exception(const std::string &resource_name, const char *file, size_t line) {
    throw std::runtime_error(ErrorFormatter<std::runtime_error>(file, line)
                             << "The resource named '" << resource_name << " (type: " << typeid(Type).name() << ")' already exisis.");
}

} // namespace details

class ResourceRegistry {
    template<typename Type>
    using ResourcePtr = std::shared_ptr<Type>;

    template<typename Type>
    using ResourceSharedFuture = std::shared_future<std::shared_ptr<Type>>;

    template<typename Type>
    using ResourceFuture = std::future<std::shared_ptr<Type>>;

    template<typename Type>
    class ResourceBlock;

public:
    template<typename Type>
    class LoadNotifyer {
        using ResourceBlock = ResourceBlock<Type>;

    public:
        LoadNotifyer(ResourceBlock *block)
            : block_{block} {
        }

        void on_loaded(const std::string &name, ResourcePtr<Type> resource) const {
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
        ResourceBlock *block_{nullptr};
    };

private:
    struct BaseResourceBlock {
        virtual ~BaseResourceBlock(){};
    };

    template<typename Type>
    struct ResourceBlock : public BaseResourceBlock {
        using DirectLoader = std::function<ResourcePtr<Type>(const std::string &)>;
        using AsyncLoader = std::function<ResourceFuture<Type>(const std::string &, LoadNotifyer<Type>)>;

        ~ResourceBlock() {}

        std::unordered_map<std::string, std::weak_ptr<Type>> dict;
        std::unordered_map<std::string, ResourceSharedFuture<Type>> loading_futures;

        // ptr -> name
        std::unordered_map<std::intptr_t, std::string> inverted_dict;

        std::mutex dict_mutex;
        std::mutex loading_futures_mutex;

        DirectLoader direct_loader;
        AsyncLoader async_loader;
    };

    struct ResourceBlockData {
        std::unique_ptr<BaseResourceBlock> block;
    };

    template<typename Type>
    ResourceBlock<Type> *resource_block_assured() {
        std::lock_guard<std::mutex> lock(resource_blocks_mutex);

        const auto index = type_seq_index<Type>::value();

        if (!(index < resource_blocks.size())) {
            resource_blocks.resize(index + 1u);
        }

        auto &&blockData = resource_blocks[index];
        if (!blockData.block) {
            blockData.block.reset(new ResourceBlock<Type>());
        }

        return static_cast<ResourceBlock<Type> *>(blockData.block.get());
    }

public:
    ResourceRegistry() = default;

    ResourceRegistry(ResourceRegistry &&) = default;

    ResourceRegistry &operator=(ResourceRegistry &&) = default;

    /**
     *
     * @param direct_loader
     *   @code{.cpp}
     *   std::shared_ptr<Type>(const std::string& name);
     *   @endcode
     *
     * @param async_loader
     *   @code{.cpp}
     *   std::future<std::shared_ptr<Type>>(const std::string& name, LoadNotifyer<Type> notifyer);
     *   @endcode
     */
    template<typename Type, typename DirectLoader, typename AsyncLoader>
    void register_resource_loader(DirectLoader &&direct_loader, AsyncLoader &&async_loader) {
        auto *block = resource_block_assured<Type>();

        block->direct_loader = direct_loader;
        block->async_loader = async_loader;
    }

    template<typename Type>
    ResourceSharedFuture<Type> get_resource(const std::string &name, LoadPolicy policy = LoadPolicy::Async) {
        auto *block = resource_block_assured<Type>();

        ResourcePtr<Type> resource;

        {
            std::lock_guard<std::mutex> lock(block->dict_mutex);
            resource = block->dict[name].lock();
        }

        if (resource) {
            std::promise<ResourcePtr<Type>> promise;
            promise.set_value(resource);

            return promise.get_future().share();
        }

        std::promise<ResourcePtr<Type>> promise;
        ResourceSharedFuture<Type> future;

        {
            std::lock_guard<std::mutex> lock(block->loading_futures_mutex);

            future = block->loading_futures[name];
            if (future.valid()) {
                // someone is loading. return future ticket.
                return future;
            }

            if (policy == LoadPolicy::Async) {
                // get future ticket from async loader immediately.
                // and register the ticket.
                future = block->async_loader(name, {block}).share();
                block->loading_futures[name] = future;

                return future;
            }

            // LoadPolicy::Direct or others
            // create future ticket immediately and register it.
            future = promise.get_future().share();
            block->loading_futures[name] = future;
        }

        // load resource directly.
        // and set resource in promise.
        resource = block->direct_loader(name);
        promise.set_value(resource);

        // for using on_loaded()
        LoadNotifyer<Type> notifyer{block};
        notifyer.on_loaded(name, resource);

        return future;
    }

    template<typename Type>
    std::pair<std::string, bool> lookup_name(ResourcePtr<Type> resource) noexcept {
        auto *block = resource_block_assured<Type>();

        std::string name;

        // get resource pointer
        auto ptr = reinterpret_cast<std::intptr_t>(resource.get());
        {
            std::lock_guard<std::mutex> lock(block->dict_mutex);

            // lookup name from the pointer
            auto iter = block->inverted_dict.find(ptr);
            if (iter == block->inverted_dict.end()) {
                return {"", false};
            }

            auto booked = block->dict[iter->second].lock();
            if (!booked || resource != booked) {
                block->inverted_dict.erase(iter);
                return {"", false};
            }
            name = iter->second;
        }
        return {name, true};
    }

private:
    std::mutex resource_blocks_mutex;
    std::vector<ResourceBlockData> resource_blocks;
};

} // namespace resource_management
} // namespace nodec

#endif