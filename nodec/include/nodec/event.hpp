#ifndef NODEC__EVENT_HPP_
#define NODEC__EVENT_HPP_

#include <nodec/nodec_exception.hpp>
#include <nodec/macros.hpp>

#include <functional>
#include <type_traits>
#include <typeinfo>
#include <memory>
#include <mutex>
#include <algorithm>
#include <unordered_map>


namespace nodec
{
/**
* @note WARNING
*   We cannnot use logging because logging using the event.
*   callbacks in logging is called infinitely.
*/
//namespace logging
//{
//void debug(const std::string& message, const char* file, size_t line);
//}

/**
* @detail
*   --- Example ---
*   // Make the shared_ptr of static callback which can take (int, int) arguments.
*   auto static_int_int_callback = StaticCallback<int, int>::make_shared(static_int_int_func);
*
*   // Create event of Callback(int, int) type.
*   Event<int, int> int_int_event;
*
*/
namespace event
{
class ObjectHasBeenDeletedException : public NodecException
{
public:

    ObjectHasBeenDeletedException(const char* file, size_t line) :
        NodecException("The owner object of callback func has been deleted.", file, line)
    {

    }

    //const char* type() const noexcept override { return "ObjectHasBeenDeletedException"; }

};
namespace detail
{
template<int>
struct placeholder_template {};

template<typename... A>
class Callback
{
public:
    template<class _Fx>
    Callback(_Fx&& func)
    {
        // Get hash code 
        // XOR(^) function pointer and type hash_code
        // XOR operator is injective function. 
        // (a1, b1) neq (a2, b2) => XOR((a1, b1)) neq XOR((a2, b2)))
        hash_ = reinterpret_cast<size_t>(&this->func) ^ (&typeid(Callback<A...>))->hash_code();
        create(func, std::make_integer_sequence<std::size_t, sizeof...(A)>{});
    }

    template<typename T, class _Fx>
    Callback(T* obj, _Fx&& func)
    {
        hash_ = reinterpret_cast<size_t>(&this->func) ^ (&typeid(Callback<A...>))->hash_code();
        create(obj, func, std::make_integer_sequence<std::size_t, sizeof...(A)>{});
        //std::cout << typeid(Callback<A...>).name();
    }

    constexpr size_t hash() const noexcept { return hash_; }
    virtual void invoke(A... args) {};

    /*
    *
    *   @note
    *       We cannot like this
    *
    *       const Callback& operator() (A... args)
    *       {
    *           invoke(args); << must unpack args in this line.
    *           return (*this);
    *       }
    *
    */
    /*
    *   @note
    *       we decided not to implement operator().
    *       if use
    *           callback_shared_ptr->(0, 1);
    *       strange?
    *       I think it better.
    *           callback_shared_ptr.invoke(0, 1);
    */
    //virtual void operator() (A... args) {};

    const bool operator==(const Callback& cb) { return (hash_ == cb.hash_); }
    const bool operator!=(const Callback& cb) { return (hash_ != cb.hash_); }


protected:
    std::function<void(A...)> func;

private:
    size_t hash_;

    template<typename T, class _Fx, std::size_t... Is>
    void create(T* obj, _Fx&& func, std::integer_sequence<std::size_t, Is...>)
    {
        // argument 1 is obj pointer
        this->func = std::function<void(A...)>(std::bind(func, obj, placeholder_template<Is>()...));
    }

    template<class _Fx, std::size_t... Is>
    void create(_Fx&& func, std::integer_sequence<std::size_t, Is... >)
    {
        // argment 1 is blank.  
        this->func = std::function<void(A...)>(std::bind(func, placeholder_template<Is>()...));
    }

    NODEC_DISABLE_COPY(Callback);

};

}

template<typename... A>
class StaticCallback : public detail::Callback<A...>
{
public:
    NODEC_SHARED_PTR_DEFINITIONS(StaticCallback);

    template<class _Fx>
    StaticCallback(_Fx&& func) :
        detail::Callback<A...>(func)
    {
    }

    void invoke(A... args) override
    {
        this->func(args...);
    }

    //void operator() (A... args) override
    //{
    //    this->func(args...);
    //}
};

template<typename T, typename... A>
class MemberCallback : public detail::Callback<A...>
{
public:
    NODEC_SHARED_PTR_DEFINITIONS(MemberCallback);

    template<class _Fx>
    MemberCallback(std::shared_ptr<T> obj, _Fx&& func) :
        detail::Callback<A...>(obj.get(), func)
    {
        // shared_ptr obj was copied, it is availble in this scope.
        // now we will refer to the obj through weak_ptr.
        this->obj = obj;
    }

    void invoke(A... args) override
    {
        if (auto obj_locked = obj.lock())
        {
            // Now we get the object not to freed in this scope.
            this->func(args...);
        }
        else
        {
            throw ObjectHasBeenDeletedException(__FILE__, __LINE__);
        }
    }

    //void operator() (A... args) override
    //{

    //}


private:
    std::weak_ptr<T> obj;

};


template<typename... A>
class Event
{
public:
    using CallbackSharedPtr = std::shared_ptr<detail::Callback<A...>>;

    Event() = default;

    void hook(CallbackSharedPtr cb)
    {
        safety_lock.lock();

        auto iter = callbacks.find(cb->hash());
        if (iter == callbacks.end())
        {
            // not found same callback
            callbacks.insert(std::pair<size_t, CallbackSharedPtr>(cb->hash(), cb));
        }
        //else
        //{
        //    std::cout << "FOUND" << std::endl;
        //}
        //auto iter = std::find_if(callbacks.begin(), callbacks.end(),
        //    [cb](CallbackSharedPtr p)
        //    {
        //        return *p == *cb;
        //    });

        //if (iter == callbacks.end())
        //{
        //    // not found same callback
        //    //callbacks.insert(std::pair<const size_t, CallbackSharedPtr>(cb->hash(), cb));
        //}

        safety_lock.unlock();
    }

    void unhook(CallbackSharedPtr cb)
    {
        safety_lock.lock();

        auto iter = callbacks.find(cb->hash());
        if (iter != callbacks.end())
        {
            // find the callback object.
            callbacks.erase(iter);
        }
        //else
        //{
        //    std::cout << "NOT FOUND" << std::endl;
        //}

        //auto iter = std::find_if(callbacks.begin(), callbacks.end(),
        //    [cb](CallbackSharedPtr p)
        //    {
        //        return *p == *cb;
        //    });

        //if (iter != callbacks.end())
        //{
        //    // find the callback object.
        //    callbacks.erase(iter);
        //}

        safety_lock.unlock();
    }

    void unhook_all()
    {
        safety_lock.lock();

        callbacks.clear();

        safety_lock.unlock();
    }

    void invoke(A... args)
    {
        safety_lock.lock();

        for (auto iter = callbacks.begin(); iter != callbacks.end();)
        {
            try
            {
                iter->second->invoke(args...);
                ++iter; // OK. nothing to problem. Next to handler.
            }
            catch (const ObjectHasBeenDeletedException& e)
            {
                // this callback func is dead, so remove it.
                iter = callbacks.erase(iter);
                //std::cout << e.what() << std::endl;
            }
            catch (...)
            {
                // Any way to continue...
                // Should handle error in each callback.
                ++iter;
                //std::cout << "?" << std::endl;
            }
        }
        //for(auto iter = callbacks.begin(); iter != callbacks.end(); ++iter)
        safety_lock.unlock();
    }

    const Event& operator+= (CallbackSharedPtr cb)
    {
        hook(cb);
        return (*this);
    }

    const Event& operator-= (CallbackSharedPtr cb)
    {
        unhook(cb);
        return (*this);
    }

private:
    std::mutex safety_lock;
    std::unordered_map<size_t, CallbackSharedPtr> callbacks;

    NODEC_DISABLE_COPY(Event);
};
}
}

namespace std
{
template<int N>
struct is_placeholder<nodec::event::detail::placeholder_template<N>> : integral_constant<int, N + 1> {};
}

#endif