#pragma once
#include <memory>
#include <utility>

namespace Nova
{
    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    
    template<typename T, typename... Args>
    constexpr SharedPtr<T> MakeShared(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    constexpr UniquePtr<T> MakeUnique(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename To, typename From, template <typename> typename SmartPtr = SharedPtr>
    constexpr SmartPtr<To> Cast(const SmartPtr<From>& Ref)
    {
        return std::dynamic_pointer_cast<To>(Ref);
    }
}
