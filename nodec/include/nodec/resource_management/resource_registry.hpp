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

public:
    template<typename Type>
    class ResourceBlock;

    template<typename Type>
    class ResourceBlockAccessor {
        using ResourceBlock = ResourceBlock<Type>;

    public:
        ResourceBlockAccessor(ResourceBlock* block)
            : block_{ block } {
        }

        void register_resource(Resource<Type> resource) {
            {
                std::lock_guard<std::mutex> lock(block_->dict_mutex);
                block_->dict[resource->name] = resource;
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
        static_assert(std::is_const_v<decltype(Type::name)>,
                      "The type of the resource name must be const.");

        using Creator = std::function<void(const std::string&)>;
        using Loader = std::function<Resource<Type>(const std::string&, ResourceBlockAccessor<Type>)>;
        using Writer = std::function<void(Resource<Type>)>;
        using Deleter = std::function<void(const std::string&)>;


        ~ResourceBlock() {}

        std::unordered_map<std::string, std::weak_ptr<Type>> dict;
        std::unordered_map<std::string, std::shared_future<Resource<Type>>> loading_futures;

        std::mutex dict_mutex;
        std::mutex loading_futures_mutex;

        Creator creator;
        Loader loader;
        Writer writer;
        Deleter deleter;
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

    template<typename Type, typename Creator, typename Loader, typename Writer, typename Deleter>
    void register_resource_handlers(Creator&& creator, Loader&& loader, Writer&& writer, Deleter&& deleter) {
        auto* block = resource_block_assured<Type>();

        block->creator = creator;
        block->loader = loader;
        block->writer = writer;
        block->deleter = deleter;
    }

    template<typename Type>
    Resource<Type> get_resource(const std::string& name) {
        auto* block = resource_block_assured<Type>();

        Resource<Type> resource;

        {
            std::lock_guard<std::mutex> lock(block->dict_mutex);
            resource = block->dict[name].lock();
        }

        if (resource) {
            return resource;
        }

        std::shared_future<Resource<Type>> future;
        std::promise<Resource<Type>> promise;

        {
            std::lock_guard<std::mutex> lock(block->loading_futures_mutex);
            auto iter = block->loading_futures.find(name);
            if (iter != block->loading_futures.end()) {
                future = iter->second;
            }
            else {
                block->loading_futures[name] = promise.get_future().share();
            }
        }

        if (future.valid()) {
            return future.get();
        }

        try {
            resource = block->loader(name, { block });
            promise.set_value(resource);
        }
        catch (...) {
            promise.set_exception(std::current_exception());
        }

        {
            std::lock_guard<std::mutex> lock(block->loading_futures_mutex);
            auto iter = block->loading_futures.find(name);
            future = iter->second;
            block->loading_futures.erase(iter);
        }

        return future.get();
    }

    template<typename Type>
    decltype(auto) new_resource(const std::string& name) {
        auto* block = resource_block_assured<Type>();

        Resource<Type> resource;
        {
            std::lock_guard<std::mutex> lock(block->dict_mutex);
            resource = block->dict[name].lock();
        }

        if (resource) {
            details::throw_resource_already_exists_exception<Type>(name, __FILE__, __LINE__);
        }

        block->creator(name);

        return get_resource<Type>(name);
    }

    template<typename Type>
    decltype(auto) apply_changes(const std::string& name) {
        auto* block = resource_block_assured<Type>();

        Resource<Type> resource;
        {
            std::lock_guard<std::mutex> lock(block->dict_mutex);
            resource = block->dict[name].lock();
        }

        if (!resource) {
            details::throw_no_resource_exception<Type>(name, __FILE__, __LINE__);
        }

        return block->writer(resource);
    }

    template<typename Type>
    decltype(auto) delete_resource(const std::string& name) {
        auto* block = resource_block_assured<Type>();

        block->deleter(name);

        {
            std::lock_guard<std::mutex> lock(block->dict_mutex);
            block->dict.erase(name);
        }

        return;
    }



private:
    std::mutex resource_blocks_mutex;
    std::vector<ResourceBlockData> resource_blocks;
};


}
}


#endif