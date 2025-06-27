#include "Mutex.h"

namespace Nova
{
    void Mutex::Lock()
    {
        m_Mutex.lock();
    }

    void Mutex::Unlock()
    {
        m_Mutex.unlock();
    }

    bool Mutex::TryLock()
    {
        return m_Mutex.try_lock();
    }

    void SharedMutex::Lock()
    {
        m_Mutex.lock();
    }

    void SharedMutex::Unlock()
    {
        m_Mutex.unlock();
    }

    bool SharedMutex::TryLock()
    {
        return m_Mutex.try_lock();
    }

    void SharedMutex::LockShared()
    {
        m_Mutex.lock_shared();
    }

    void SharedMutex::UnlockShared()
    {
        m_Mutex.unlock_shared();
    }

    bool SharedMutex::TryLockShared()
    {
        return m_Mutex.try_lock_shared();
    }

    void RecursiveMutex::Lock()
    {
        m_Mutex.lock();
    }

    void RecursiveMutex::Unlock()
    {
        m_Mutex.unlock();
    }

    bool RecursiveMutex::TryLock()
    {
        return m_Mutex.try_lock();
    }
}
