#pragma once
#include <memory>
#include <utility>

namespace Nova
{
    template<typename T>
    using SharedPtr = std::shared_ptr<T>;
    
    template<typename T, typename... Args>
    constexpr SharedPtr<T> MakeShared(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename To, typename From>
    constexpr SharedPtr<To> Cast(const SharedPtr<From>& Ref)
    {
        return std::dynamic_pointer_cast<To>(Ref);
    }
}
