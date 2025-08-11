#pragma once
#include "RefObject.h"
#include <type_traits>

namespace Nova
{
    template <typename T>
    class Ref
    {
    public:
        Ref() = default;

        Ref(T* ptr) : m_Pointer(ptr)
        {
            if (ptr)
                ((RefObject*)ptr)->AddReference();
        }

        Ref(decltype(nullptr)) : m_Pointer(nullptr) {}

        Ref(const Ref& other) : m_Pointer(other.m_Pointer)
        {
            if (other.m_Pointer)
                ((RefObject*)other.m_Pointer)->AddReference();
        }

        Ref(Ref&& other) noexcept : m_Pointer(other.m_Pointer)
        {
            other.m_Pointer = nullptr;
        }

        template <typename U> requires std::is_base_of_v<T, U>
        Ref(const Ref<U>& other) : m_Pointer((T*)other.m_Pointer)
        {
            if (other.m_Pointer)
                ((RefObject*)other.m_Pointer)->AddReference();
        }

        ~Ref()
        {
            if (m_Pointer)
                ((RefObject*)m_Pointer)->ReleaseReference();
        }

        void operator=(const Ref& other)
        {
            T* old = m_Pointer;
            if (other.m_Pointer)
                ((RefObject*)other.m_Pointer)->AddReference();
            m_Pointer = other.m_Pointer;
            if (old)
                ((RefObject*)old)->ReleaseReference();
        }

        void operator=(Ref&& other) noexcept
        {
            T* old = m_Pointer;
            m_Pointer = other.m_Pointer;
            other.m_Pointer = old;
        }

        template <typename U>
        void operator=(const Ref<U>& other)
        {
            T* old = m_Pointer;
            if (other.m_Pointer)
                ((RefObject*)other.m_Pointer)->AddReference();
            m_Pointer = (T*)other.m_Pointer;
            if (old)
                ((RefObject*)old)->ReleaseReference();
        }

        void operator=(decltype(nullptr))
        {
            if (m_Pointer)
                ((RefObject*)m_Pointer)->ReleaseReference();
            m_Pointer = nullptr;
        }

        bool operator==(const T* ptr) const { return m_Pointer == ptr; }
        bool operator!=(const T* ptr) const { return m_Pointer != ptr; }
        bool operator==(const Ref& ptr) const { return m_Pointer == ptr.m_Pointer; }
        bool operator!=(const Ref& ptr) const { return m_Pointer != ptr.m_Pointer; }

        template <typename U>
        Ref<U> As() const
        {
            return Ref<U>(dynamic_cast<U*>(m_Pointer));
        }


        T& operator*() const { return *m_Pointer; }
        T* operator->() const { return m_Pointer; }
        T* Get() const { return m_Pointer; }

        operator T*() const { return m_Pointer; }

        template <typename U>
        operator U*() const { return (U*)m_Pointer; }

        void Attach(T* newPointer)
        {
            T* old = m_Pointer;
            m_Pointer = newPointer;
            if (old)
                ((RefObject*)old)->ReleaseReference();
        }

        T* Detach()
        {
            T* result = m_Pointer;
            m_Pointer = nullptr;
            return result;
        }

        void Swap(Ref& other)
        {
            T* otherPointer = other.m_Pointer;
            other.m_Pointer = m_Pointer;
            m_Pointer = otherPointer;
        }

    private:
        T* m_Pointer = nullptr;

        template <typename U>
        friend class Ref;
    };

    template <typename T, typename... Args> requires std::is_base_of_v<RefObject, T>
    Ref<T> MakeRef(Args&&... args)
    {
        return Ref<T>(new T(std::forward<Args>(args)...));
    }
}
