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
        using ReferenceType = T&;
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

        template <typename U> requires std::is_base_of_v<T, U>
        Ref(const Ref<U>& other) : m_Pointer((PointerType)other.m_Pointer)
        {
            AddRef(other.m_Pointer);
        }

        ~Ref()
        {
            RelRef(m_Pointer);
        }

        void operator=(const Ref& other)
        {
            PointerType old = m_Pointer;
            AddRef(other.m_Pointer);
            m_Pointer = other.m_Pointer;
            RelRef(old);
        }

        void operator=(Ref&& other) noexcept
        {
            PointerType old = m_Pointer;
            m_Pointer = other.m_Pointer;
            other.m_Pointer = old;
        }

        template <typename U>
        void operator=(const Ref<U>& other)
        {
            PointerType old = m_Pointer;
            AddRef(other.m_Pointer);
            m_Pointer = (PointerType)other.m_Pointer;
            RelRef(old);
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
        const ReferenceType operator*() const { return *m_Pointer; }

        PointerType operator->() { return m_Pointer; }
        const PointerType operator->() const { return m_Pointer; }

        PointerType Get() { return m_Pointer; }
        const PointerType Get() const { return m_Pointer; }

        operator PointerType() { return m_Pointer; }
        operator const PointerType() const { return m_Pointer; }

        template <typename U>
        operator U*() const { return (U*)m_Pointer; }


        void Attach(PointerType newPointer)
        {
            PointerType old = m_Pointer;
            m_Pointer = newPointer;
            RelRef(old);
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
    };

    template <typename T, typename... Args> requires std::is_base_of_v<RefObject, T>
    Ref<T> MakeRef(Args&&... args)
    {
        return Ref<T>(new T(std::forward<Args>(args)...));
    }


    template<typename T> requires std::is_base_of_v<RefObject, T>
    class WeakRef
    {
    public:
        using RefType = Ref<T>;
        using PointerType = typename RefType::PointerType;
        using ReferenceType = typename RefType::ReferenceType;

        explicit WeakRef(RefType& ref) : m_Pointer(ref.Get()){}

        template<typename U> requires std::is_base_of_v<RefObject, U>
        explicit WeakRef(Ref<U>& ref) : m_Pointer(PointerType(ref.Get())){}

        WeakRef(decltype(nullptr)) : m_Pointer(nullptr) {}

        void operator=(decltype(nullptr)){ m_Pointer = nullptr; }

        bool operator==(const PointerType ptr) const { return m_Pointer == ptr; }
        bool operator!=(const PointerType ptr) const { return m_Pointer != ptr; }
        bool operator==(const WeakRef& ptr) const { return m_Pointer == ptr.m_Pointer; }
        bool operator!=(const WeakRef& ptr) const { return m_Pointer != ptr.m_Pointer; }

        template <typename U>
        WeakRef<U> As() const
        {
            return WeakRef<U>(dynamic_cast<U*>(m_Pointer));
        }


        ReferenceType operator*() { return *m_Pointer; }
        const ReferenceType operator*() const { return *m_Pointer; }

        PointerType operator->() { return m_Pointer; }
        const PointerType operator->() const { return m_Pointer; }

        PointerType Get() { return m_Pointer; }
        const PointerType Get() const { return m_Pointer; }

        operator PointerType() { return m_Pointer; }
        operator const PointerType() const { return m_Pointer; }
    private:
        PointerType m_Pointer = nullptr;
    };
}
