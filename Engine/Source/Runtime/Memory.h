﻿#pragma once
#include <cstring>
#include "Runtime/Types.h"

#define NOVA_MALLOC(Size) ::operator new []((Size))
#define NOVA_REALLOC(Ptr, Size) ::operator new []((Size), (Ptr))
#define NOVA_MEMSET(Ptr, Value, Size) std::memset((Ptr), (Value), (Size))
#define NOVA_FREE(Ptr) ::operator delete[]((Ptr))

namespace Nova::Memory
{
    template<typename T>
    T* Malloc(size_t Count = 1)
    {
        return (T*)NOVA_MALLOC(Count * sizeof(T));
    }
    
    template<typename T>
    T* Realloc(T* Ptr, size_t Count = 1)
    {
        return (T*)NOVA_REALLOC(Ptr, Count * sizeof(T));
    }

    template<typename T>
    T* Memset(T* Ptr, i32 Value, size_t Count = 1)
    {
        return (T*)NOVA_MEMSET(Ptr, Value, Count * sizeof(T));
    }

    template<typename T>
    T* Memzero(T* Ptr)
    {
        return Memset(Ptr, 0, 1);
    }

    template<typename T>
    T* Memzero(T& Ptr)
    {
        return Memset(&Ptr, 0, 1);
    }

    template<typename T>
    T* Calloc(size_t Count = 1)
    {
        T* Result = Malloc<T>(Count);
        Result = Memset(Result, 0, Count);
        return Result;
    }

    template<typename T>
    void Free(T* Ptr)
    {
        NOVA_FREE(Ptr);
    }
}
