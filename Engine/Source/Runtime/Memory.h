#pragma once
#include <cstdint>
#include <cstring>

#define NOVA_MALLOC(size) ::operator new []((size))
#define NOVA_REALLOC(ptr, size) ::operator new []((size), (ptr))
#define NOVA_MEMSET(ptr, value, size) std::memset((ptr), (value), (size))
#define NOVA_FREE(ptr) ::operator delete[]((ptr))

namespace Nova::Memory
{
    template<typename T>
    T* Malloc(const size_t count = 1)
    {
        return (T*)NOVA_MALLOC(count * sizeof(T));
    }
    
    template<typename T>
    T* Realloc(T* ptr, const size_t count = 1)
    {
        return (T*)NOVA_REALLOC(ptr, count * sizeof(T));
    }

    template<typename T>
    T* Memset(T* ptr, const int32_t value, const size_t count = 1)
    {
        return (T*)NOVA_MEMSET((void*)ptr, value, count * sizeof(T));
    }

    template<typename T>
    T* Memzero(T* ptr)
    {
        return Memset(ptr, 0, 1);
    }

    template<typename T>
    T* Memzero(T& ptr)
    {
        return Memset(&ptr, 0, 1);
    }

    template<typename T>
    T* Calloc(size_t count = 1)
    {
        T* result = Malloc<T>(count);
        result = Memset(result, 0, count);
        return result;
    }

    template<typename T>
    void Free(T* ptr)
    {
        NOVA_FREE(ptr);
    }

    template<typename SourceType, typename DestType>
    void Memcpy(DestType* dest, const SourceType* src, const size_t size)
    {
        memcpy(dest, src, size);
    }

    inline bool Memcmp(const void* lhs, const void* rhs, const size_t size)
    {
        return memcmp(lhs, rhs, size) == 0;
    }
}
