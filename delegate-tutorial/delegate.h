#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>
#include <mutex>

// Primary template declaration
template<typename>
class Delegate;

// Template specialization that parses function signature
template<typename ReturnType, typename... Args>
class Delegate<ReturnType(Args...)>
{
    // Base class for delegate function wrappers
    class _delegate_wrapper_base
    {
    public:
        virtual ~_delegate_wrapper_base() = default;
        virtual ReturnType invoke(Args... args) = 0;
        virtual bool equals(const _delegate_wrapper_base *other) const = 0;
        virtual bool isSameObject(const void *obj) const = 0;
        virtual std::shared_ptr<_delegate_wrapper_base> clone() const = 0;
    };

    // Wrapper for static functions and lambdas
    template<typename T>
    class _delegate_wrapper : public _delegate_wrapper_base
    {
    public:
        explicit _delegate_wrapper(T callable) : callable_(std::move(callable)) {}

        ReturnType invoke(Args... args) override
        {
            if constexpr (std::is_same_v<ReturnType, void>)
            {
                callable_(std::forward<Args>(args)...);
            }
            else
            {
                return callable_(std::forward<Args>(args)...);
            }
        }

        bool equals(const _delegate_wrapper_base *other) const override
        {
            if (auto derived = dynamic_cast<const _delegate_wrapper<T> *>(other))
            {
                if constexpr (std::is_pointer_v<T>)
                {
                    return callable_ == derived->callable_;
                }
                else
                {
                    return false;
                }
            }
            return false;
        }

        bool isSameObject(const void *obj) const override
        {
            return false;
        }

        std::shared_ptr<_delegate_wrapper_base> clone() const override
        {
            return std::make_shared<_delegate_wrapper<T>>(callable_);
        }

    private:
        T callable_;
    };

    // Wrapper for member functions (using shared_ptr)
    template<typename T, typename MemberFunc>
    class _member_delegate_wrapper : public _delegate_wrapper_base
    {
    public:
        _member_delegate_wrapper(std::shared_ptr<T> object, MemberFunc memberFunc)
            : object_(std::move(object)), memberFunc_(memberFunc)
        {
        }

        ReturnType invoke(Args... args) override
        {
            if (object_)
            {
                if constexpr (std::is_same_v<ReturnType, void>)
                {
                    std::invoke(memberFunc_, object_.get(), std::forward<Args>(args)...);
                }
                else
                {
                    return std::invoke(memberFunc_, object_.get(), std::forward<Args>(args)...);
                }
            }
            else if constexpr (!std::is_same_v<ReturnType, void>)
            {
                return ReturnType{};
            }
        }

        bool equals(const _delegate_wrapper_base *other) const override
        {
            if (auto derived = dynamic_cast<const _member_delegate_wrapper<T, MemberFunc> *>(other))
            {
                return object_ == derived->object_ && memberFunc_ == derived->memberFunc_;
            }
            return false;
        }

        bool isSameObject(const void *obj) const override
        {
            return object_.get() == obj;
        }

        std::shared_ptr<_delegate_wrapper_base> clone() const override
        {
            return std::make_shared<_member_delegate_wrapper<T, MemberFunc>>(object_, memberFunc_);
        }

    private:
        std::shared_ptr<T> object_;
        MemberFunc memberFunc_;
    };

    // Wrapper for member functions (using raw pointer)
    template<typename T, typename MemberFunc>
    class _raw_member_delegate_wrapper : public _delegate_wrapper_base
    {
    public:
        _raw_member_delegate_wrapper(T *object, MemberFunc memberFunc) : object_(object), memberFunc_(memberFunc) {}

        ReturnType invoke(Args... args) override
        {
            if (object_)
            {
                if constexpr (std::is_same_v<ReturnType, void>)
                {
                    std::invoke(memberFunc_, object_, std::forward<Args>(args)...);
                }
                else
                {
                    return std::invoke(memberFunc_, object_, std::forward<Args>(args)...);
                }
            }
            else if constexpr (!std::is_same_v<ReturnType, void>)
            {
                return ReturnType{};
            }
        }

        bool equals(const _delegate_wrapper_base *other) const override
        {
            if (auto derived = dynamic_cast<const _raw_member_delegate_wrapper<T, MemberFunc> *>(other))
            {
                return object_ == derived->object_ && memberFunc_ == derived->memberFunc_;
            }
            return false;
        }

        bool isSameObject(const void *obj) const override
        {
            return object_ == obj;
        }

        std::shared_ptr<_delegate_wrapper_base> clone() const override
        {
            return std::make_shared<_raw_member_delegate_wrapper<T, MemberFunc>>(object_, memberFunc_);
        }

    private:
        T *object_;
        MemberFunc memberFunc_;
    };

    mutable std::mutex mutex_;
    std::vector<std::shared_ptr<_delegate_wrapper_base>> delegates;

public:
    Delegate() = default;

    // Add static function or lambda
    template<typename T>
    void add(T callable)
    {
        static_assert(std::is_invocable_r_v<ReturnType, T, Args...>,
                      "Callable signature does not match delegate signature");
        std::lock_guard<std::mutex> lock(mutex_);
        delegates.push_back(std::make_shared<_delegate_wrapper<T>>(std::move(callable)));
    }

    // Add non-const member function (using shared_ptr)
    template<typename T, typename Ret, typename... MemberArgs>
    void add(std::shared_ptr<T> object, Ret (T::*memberFunc)(MemberArgs...))
    {
        static_assert(std::is_invocable_r_v<ReturnType, decltype(memberFunc), T *, Args...>,
                      "Member function signature does not match delegate signature");
        std::lock_guard<std::mutex> lock(mutex_);
        delegates.push_back(
                std::make_shared<_member_delegate_wrapper<T, decltype(memberFunc)>>(std::move(object), memberFunc));
    }

    // Add non-const member function (using raw pointer)
    template<typename T, typename Ret, typename... MemberArgs>
    void add(T *object, Ret (T::*memberFunc)(MemberArgs...))
    {
        static_assert(std::is_invocable_r_v<ReturnType, decltype(memberFunc), T *, Args...>,
                      "Member function signature does not match delegate signature");
        std::lock_guard<std::mutex> lock(mutex_);
        delegates.push_back(
                std::make_shared<_raw_member_delegate_wrapper<T, decltype(memberFunc)>>(object, memberFunc));
    }

    // Add const member function (using shared_ptr)
    template<typename T, typename Ret, typename... MemberArgs>
    void add(std::shared_ptr<T> object, Ret (T::*memberFunc)(MemberArgs...) const)
    {
        static_assert(std::is_invocable_r_v<ReturnType, decltype(memberFunc), const T *, Args...>,
                      "Const member function signature does not match delegate signature");
        std::lock_guard<std::mutex> lock(mutex_);
        delegates.push_back(std::make_shared<_member_delegate_wrapper<const T, decltype(memberFunc)>>(std::move(object),
                                                                                                      memberFunc));
    }

    // Add const member function (using raw pointer)
    template<typename T, typename Ret, typename... MemberArgs>
    void add(const T *object, Ret (T::*memberFunc)(MemberArgs...) const)
    {
        static_assert(std::is_invocable_r_v<ReturnType, decltype(memberFunc), const T *, Args...>,
                      "Const member function signature does not match delegate signature");
        std::lock_guard<std::mutex> lock(mutex_);
        delegates.push_back(
                std::make_shared<_raw_member_delegate_wrapper<const T, decltype(memberFunc)>>(object, memberFunc));
    }

    // Remove delegate
    template<typename T>
    void remove(T callable)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        _delegate_wrapper<T> wrapper(std::move(callable));
        auto it = std::remove_if(delegates.begin(), delegates.end(),
                                 [&wrapper](const std::shared_ptr<_delegate_wrapper_base> &delegate) {
                                     return delegate->equals(&wrapper);
                                 });
        delegates.erase(it, delegates.end());
    }

    // Remove all delegates for specific object
    template<typename T>
    void remove_all_for_object(const T *obj)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::remove_if(
                delegates.begin(), delegates.end(),
                [obj](const std::shared_ptr<_delegate_wrapper_base> &delegate) { return delegate->isSameObject(obj); });
        delegates.erase(it, delegates.end());
    }

    // Broadcast to all delegates (void return version)
    template<typename R = ReturnType>
    std::enable_if_t<std::is_same_v<R, void>, void> broadcast(Args... args)
    {
        for (const auto &delegate: delegates)
        {
            delegate->invoke(std::forward<Args>(args)...);
        }
    }

    // Broadcast to all delegates (non-void return version)
    template<typename R = ReturnType>
    std::enable_if_t<!std::is_same_v<R, void>, std::vector<R>> broadcast(Args... args)
    {
        std::vector<R> results;
        results.reserve(delegates.size());
        for (const auto &delegate: delegates)
        {
            results.push_back(delegate->invoke(std::forward<Args>(args)...));
        }
        return results;
    }

    // Safe broadcast (prevents modification during invocation)
    template<typename R = ReturnType>
    std::enable_if_t<std::is_same_v<R, void>, void> safe_broadcast(Args... args)
    {
        auto delegatesCopy = delegates;
        for (const auto &delegate: delegatesCopy)
        {
            delegate->invoke(std::forward<Args>(args)...);
        }
    }

    template<typename R = ReturnType>
    std::enable_if_t<!std::is_same_v<R, void>, std::vector<R>> safe_broadcast(Args... args)
    {
        auto delegatesCopy = delegates;
        std::vector<R> results;
        results.reserve(delegatesCopy.size());
        for (const auto &delegate: delegatesCopy)
        {
            results.push_back(delegate->invoke(std::forward<Args>(args)...));
        }
        return results;
    }

    // Operator() overload (void return version)
    template<typename R = ReturnType>
    std::enable_if_t<std::is_same_v<R, void>, void> operator()(Args... args)
    {
        broadcast(std::forward<Args>(args)...);
    }

    // Operator() overload (non-void return version)
    template<typename R = ReturnType>
    std::enable_if_t<!std::is_same_v<R, void>, std::vector<R>> operator()(Args... args)
    {
        return broadcast(std::forward<Args>(args)...);
    }

    // Invoke only the first delegate
    std::optional<ReturnType> invoke_front(Args... args)
    {
        if (!delegates.empty())
        {
            if constexpr (std::is_same_v<ReturnType, void>)
            {
                delegates.front()->invoke(std::forward<Args>(args)...);
                return std::nullopt;
            }
            else
            {
                return delegates.front()->invoke(std::forward<Args>(args)...);
            }
        }
        return std::nullopt;
    }

    // Invoke delegates until condition is met
    template<typename Condition>
    std::optional<ReturnType> invoke_until(Condition condition, Args... args)
    {
        for (const auto &delegate: delegates)
        {
            if constexpr (std::is_same_v<ReturnType, void>)
            {
                delegate->invoke(std::forward<Args>(args)...);
                if (condition())
                    return std::nullopt;
            }
            else
            {
                auto result = delegate->invoke(std::forward<Args>(args)...);
                if (condition(result))
                    return result;
            }
        }
        return std::nullopt;
    }

    // Clear all delegates
    void clear() noexcept
    {
        delegates.clear();
    }

    // Get number of delegates
    size_t size() const noexcept
    {
        return delegates.size();
    }

    // Check if delegate is empty
    bool empty() const noexcept
    {
        return delegates.empty();
    }

    // Check if delegate contains specific object
    template<typename T>
    bool contains(const T *obj) const
    {
        for (const auto &delegate: delegates)
        {
            if (delegate->isSameObject(obj))
            {
                return true;
            }
        }
        return false;
    }

    // Support copying
    Delegate(const Delegate &other)
    {
        std::lock_guard<std::mutex> lock(other.mutex_);
        for (const auto &delegate: other.delegates)
        {
            delegates.push_back(delegate->clone());
        }
    }

    Delegate &operator=(const Delegate &other)
    {
        if (this != &other)
        {
            std::scoped_lock lock(mutex_, other.mutex_);

            delegates.clear();
            for (const auto &delegate: other.delegates)
            {
                delegates.push_back(delegate->clone());
            }
        }
        return *this;
    }

    // Support moving
    Delegate(Delegate &&) = default;
    Delegate &operator=(Delegate &&) = default;
};