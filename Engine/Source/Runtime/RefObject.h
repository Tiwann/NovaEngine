#pragma once
#include <atomic>

namespace Nova
{
    class RefObject
    {
    public:
        RefObject() = default;
        RefObject(const RefObject&) { }
        virtual ~RefObject() = default;

        RefObject& operator=(const RefObject&) { return *this; }

        uint32_t AddReference()
        {
            const uint32_t count = m_RefCount.fetch_add(1);
            const uint32_t internalCount = m_InternalRefCount.load();
            if (internalCount > 0 && count == internalCount)
                MakeExternal();
            return count + 1;
        }

        uint32_t ReleaseReference()
        {
            const uint32_t count = m_RefCount.fetch_sub(1);
            const uint32_t internalCount = m_InternalRefCount.load();

            if (internalCount > 0 && count == internalCount + 1)
                MakeInternal();

            if (count == 1)
                delete this;
            return count - 1;
        }

        uint32_t RefCount() const { return m_RefCount; }

        virtual void MakeExternal(){}
        virtual void MakeInternal(){}
    private:
        std::atomic<uint32_t> m_RefCount = 0;
        std::atomic<uint32_t> m_InternalRefCount = 0;
    };
}
