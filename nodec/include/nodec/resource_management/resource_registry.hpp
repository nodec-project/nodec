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
    using ResourceFuture = std::shared_future<Resource<Type>>;

    template<typename Type>
    class ResourceBlock;

    template<typename Type>
    class ResourceBlockAccess {
        using ResourceBlock = ResourceBlock<Type>;

    public:
        ResourceBlockAccess(ResourceBlock* block)
            : block_{ block } {
        }

        void register_resource(Resource<Type> resource) {
            std::lock_guard<std::mutex> lock(block_->dict_mutex);
            block_->dict[resource->name] = resource;
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

        using Creator = std::function<ResourceFuture<Type>(const std::string&, ResourceBlockAccess<Type>)>;
        using Loader = std::function<ResourceFuture<Type>(const std::string&, ResourceBlockAccess<Type>)>;
        using Writer = std::function<std::future<bool>(Resource<Type>)>;


        ~ResourceBlock() {}

        std::unordered_map<std::string, std::weak_ptr<Type>> dict;

        std::mutex dict_mutex;

        Creator creator;
        Loader loader;
        Writer writer;
    };

    struct ResourceBlockData {
        std::unique_ptr<BaseResourceBlock> block;
    };


    template<typename Type>
    ResourceBlock<Type>* resource_block_assured() {
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


    template<typename Type>
    ResourceFuture<Type> get_resource(const std::string& name) {
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

        return block->loader(name, { block });
    }

    template<typename Type>
    ResourceFuture<Type> new_resource(const std::string& name) {
        auto* block = resource_block_assured<Type>();

        Resource<Type> resource;
        {
            std::lock_guard<std::mutex> lock(block->dict_mutex);
            resource = block->dict[name].lock();
        }

        if (resource) {
            details::throw_resource_already_exists_exception<Type>(name, __FILE__, __LINE__);
        }

        return block->creator(name, { block });
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

    template<typename Type, typename Creator, typename Loader, typename Writer>
    void register_resource_handlers(Creator&& creator, Loader&& loader, Writer&& writer) {
        auto* block = resource_block_assured<Type>();

        block->creator = creator;
        block->loader = loader;
        block->writer = writer;
    }

private:
    std::vector<ResourceBlockData> resource_blocks;
};


}
}


#endif