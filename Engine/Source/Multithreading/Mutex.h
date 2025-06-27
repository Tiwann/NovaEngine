#pragma once
#include "Runtime/Definitions.h"
#include <mutex>
#include <shared_mutex>

namespace Nova
{
    class Mutex
    {
    public:
        Mutex() = default;
        ~Mutex() = default;
        Mutex& operator=(const Mutex&) = delete;

        NOVA_FORCEINLINE void Lock();
        NOVA_FORCEINLINE void Unlock();
        NOVA_FORCEINLINE bool TryLock();

        NOVA_FORCEINLINE operator std::mutex&() { return m_Mutex; };
    private:
        std::mutex m_Mutex;
    };

    class SharedMutex
    {
    public:
        SharedMutex() = default;
        ~SharedMutex() = default;
        Mutex& operator=(const Mutex&) = delete;

        NOVA_FORCEINLINE void Lock();
        NOVA_FORCEINLINE void Unlock();
        NOVA_FORCEINLINE bool TryLock();
        NOVA_FORCEINLINE void LockShared();
        NOVA_FORCEINLINE void UnlockShared();
        NOVA_FORCEINLINE bool TryLockShared();

        NOVA_FORCEINLINE operator std::shared_mutex&() { return m_Mutex; };
    private:
        std::shared_mutex m_Mutex;
    };

    class RecursiveMutex
    {
    public:
        RecursiveMutex() = default;
        ~RecursiveMutex() = default;
        RecursiveMutex& operator=(const Mutex&) = delete;

        NOVA_FORCEINLINE void Lock();
        NOVA_FORCEINLINE void Unlock();
        NOVA_FORCEINLINE bool TryLock();

        NOVA_FORCEINLINE operator std::recursive_mutex&() { return m_Mutex; };
    private:
        std::recursive_mutex m_Mutex;
    };
}
