#pragma once
#include <memory>
#include <utility>

namespace Nova
{
    template<typename T>
    using Ref = std::shared_ptr<T>;
    
    template<typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename To, typename From>
    constexpr Ref<To> Cast(const Ref<From>& Ref)
    {
        return std::dynamic_pointer_cast<To>(Ref);
    }
}
