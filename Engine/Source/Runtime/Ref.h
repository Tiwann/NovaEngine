#pragma once
#include "RefObject.h"
#include <type_traits>

namespace Nova
{
    template <typename T>
    class Ref
    {
    public:
        using PointerType = T*;
        using ConstPointerType = const T*;
        using ReferenceType = T&;
        using ConstReferenceType = const T&;

        Ref() = default;

        Ref(PointerType ptr) : m_Pointer(ptr)
        {
            AddRef(ptr);
        }

        Ref(decltype(nullptr)) : m_Pointer(nullptr) {}

        Ref(const Ref& other) : m_Pointer(other.m_Pointer)
        {
            AddRef(other.m_Pointer);
        }

        Ref(Ref&& other) noexcept : m_Pointer(other.m_Pointer)
        {
            other.m_Pointer = nullptr;
        }

        template <typename U>
        Ref(const Ref<U>& other) : m_Pointer(static_cast<PointerType>(other.m_Pointer))
        {
            AddRef(other.m_Pointer);
        }

        ~Ref()
        {
            if (m_Pointer)
                RelRef(m_Pointer);
        }

        Ref& operator=(const Ref& other)
        {
            if (this == &other)
                return *this;

            PointerType old = m_Pointer;
            AddRef(other.m_Pointer);
            m_Pointer = other.m_Pointer;
            RelRef(old);
            return *this;
        }

        Ref& operator=(Ref&& other) noexcept
        {
            if (this == &other)
                return *this;

            RelRef(m_Pointer);

            m_Pointer = other.m_Pointer;
            other.m_Pointer = nullptr;

            return *this;
        }

        template <typename U> requires (!std::is_same_v<T, U>)
        Ref& operator=(const Ref<U>& other)
        {
            PointerType old = m_Pointer;
            AddRef(other.m_Pointer);
            m_Pointer = other.m_Pointer;
            RelRef(old);
            return *this;
        }

        template <typename U> requires (!std::is_same_v<T, U>)
        Ref& operator=(Ref<U>&& other)
        {
            if (this == &other)
                return *this;

            RelRef(m_Pointer);

            m_Pointer = other.m_Pointer;
            other.m_Pointer = nullptr;

            return *this;
        }

        void operator=(decltype(nullptr))
        {
            RelRef(m_Pointer);
            m_Pointer = nullptr;
        }

        bool operator==(const PointerType ptr) const { return m_Pointer == ptr; }
        bool operator!=(const PointerType ptr) const { return m_Pointer != ptr; }
        bool operator==(const Ref& ptr) const { return m_Pointer == ptr.m_Pointer; }
        bool operator!=(const Ref& ptr) const { return m_Pointer != ptr.m_Pointer; }

        template <typename U>
        Ref<U> As() const
        {
            return Ref<U>(dynamic_cast<U*>(m_Pointer));
        }


        ReferenceType operator*() { return *m_Pointer; }
        ConstReferenceType operator*() const { return *m_Pointer; }

        PointerType operator->() { return m_Pointer; }
        ConstPointerType operator->() const { return m_Pointer; }

        PointerType Get() { return m_Pointer; }
        ConstPointerType Get() const { return m_Pointer; }

        operator PointerType() { return m_Pointer; }
        operator ConstPointerType() const { return m_Pointer; }
        explicit operator bool() const { return m_Pointer; }

        //template <typename U>
        //operator const U*() const { return (U*)m_Pointer; }

        //template <typename U>
        //operator U*() { return (U*)m_Pointer; }


        void Attach(PointerType newPointer)
        {
            AddRef(newPointer);
            RelRef(m_Pointer);
            m_Pointer = newPointer;
        }

        PointerType Detach()
        {
            PointerType result = m_Pointer;
            m_Pointer = nullptr;
            return result;
        }

        void Swap(Ref& other)
        {
            PointerType otherPointer = other.m_Pointer;
            other.m_Pointer = m_Pointer;
            m_Pointer = otherPointer;
        }
    private:
        friend class AssetDatabase;

        void Release()
        {
            KillRef(m_Pointer);
            m_Pointer = nullptr;
        }
        mutable PointerType m_Pointer = nullptr;

        template <typename U>
        friend class Ref;
    };

    template <typename T, typename... Args> requires std::is_base_of_v<RefObject, T>
    Ref<T> MakeRef(Args&&... args)
    {
        return Ref<T>(new T(std::forward<Args>(args)...));
    }
}
