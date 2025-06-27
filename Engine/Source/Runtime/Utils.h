#pragma once

namespace Nova
{
    template<size_t N>
    consteval size_t KB()
    {
        return N * 1024;
    }

    template<size_t N>
    consteval size_t MB()
    {
        return KB<N>() * 1024;
    }

    template<size_t N>
    consteval size_t GB()
    {
        return MB<N>() * 1024;
    }

    template<typename T, size_t N>
    constexpr size_t ArrayCount(const T(&)[N])
    {
        return N;
    }
}