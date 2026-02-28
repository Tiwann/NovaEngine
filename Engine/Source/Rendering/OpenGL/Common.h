#pragma once
#include <cstdint>
#include <type_traits>

namespace Nova::OpenGL
{
    template<typename T> requires std::is_integral_v<T>
    static constexpr T INVALID_HANDLE = T(0xFFFFFFFF);

    static constexpr bool HandleIsValid(const uint32_t handle)
    {
        return handle != INVALID_HANDLE<uint32_t>;
    }

    template<typename T, uint32_t N> requires std::is_integral_v<T>
    static bool HandlesAreValid(const T(&handles)[N])
    {
        for (uint32_t i = 0; i < N; i++)
            if (!HandleIsValid(handles[i]))
                return false;
        return true;
    }

    template<typename... Args> requires (std::is_integral_v<Args> && ...)
    static bool HandlesAreValid(Args&&... handles)
    {
        return (HandleIsValid(std::forward<Args>(handles)) && ...);
    }
}
