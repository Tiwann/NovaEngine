#include "Thread.h"

namespace Nova
{
    void Thread::Detach()
    {
        m_Thread.detach();
    }

    void Thread::Join()
    {
        m_Thread.join();
    }

    bool Thread::Joinable() const
    {
        return m_Thread.joinable();
    }
}
