#pragma once
#include "RefCounted.h"
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
        using PointerToConst = T* const;
        using ConstPointerToConst = const T* const;
        using ValueType = T;

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
        Ref(const Ref<U>& other) : m_Pointer(reinterpret_cast<PointerType>(other.m_Pointer))
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

        void Release()
        {
            KillRef(m_Pointer);
            m_Pointer = nullptr;
        }
    private:
        PointerType m_Pointer = nullptr;

        template <typename U>
        friend class Ref;

        template <typename U>
        friend class WeakRef;
    };

    template<typename T>
    class WeakRef
    {
    public:
        using PointerType = Ref<T>::PointerType;
        using ConstPointerType = Ref<T>::ConstPointerType;
        using ReferenceType = Ref<T>::ReferenceType;
        using ConstReferenceType = Ref<T>::ConstReferenceType;

        WeakRef(const Ref<T>& ref) : m_Pointer(ref.m_Pointer) {}
        WeakRef(const WeakRef&) = default;
        WeakRef(WeakRef&&) = default;
        WeakRef& operator=(const WeakRef&) = default;
        WeakRef& operator=(WeakRef&&) = default;
        ~WeakRef() = default;

        operator PointerType() { return m_Pointer; }
        operator ConstPointerType() const { return m_Pointer; }

        ReferenceType operator*() { return *m_Pointer; }
        ConstReferenceType operator*() const { return *m_Pointer; }

        PointerType operator->() { return m_Pointer; }
        ConstPointerType operator->() const { return m_Pointer; }

        PointerType Get() { return m_Pointer; }
        ConstPointerType Get() const { return m_Pointer; }

        explicit operator bool() const { return m_Pointer; }
    private:
        PointerType m_Pointer = nullptr;
    };

    template <typename T, typename... Args> requires std::is_base_of_v<RefCounted, T>
    Ref<T> MakeRef(Args&&... args)
    {
        return Ref<T>(new T(std::forward<Args>(args)...));
    }

    template<typename T>
    WeakRef<T> MakeWeak(const Ref<T>& ref) { return WeakRef<T>(ref); }
}
