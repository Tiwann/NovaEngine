#pragma once

namespace Nova
{
    template <typename T>
    class ScopedPointer
    {
    public:
        using PointerType = T*;
        using ReferenceType = T&;
        
        ScopedPointer(PointerType Obj) : m_Data(Obj) {}

        template <typename U> requires IsBaseOfValue<T, U>
        ScopedPointer(ScopedPointer<U>&& Other) : m_Data(std::move(Other.Get())) {}

        ~ScopedPointer()
        {
            delete m_Data;
        }

        PointerType Get() { return m_Data; }
        const PointerType Get() const { return m_Data; }

        operator PointerType() { return m_Data; }
        operator const PointerType() const  { return m_Data; }

        PointerType operator->() { return m_Data; }
        const PointerType operator->() const { return m_Data; }

        ReferenceType operator*() { return *m_Data; }
        const ReferenceType operator*() const { return *m_Data; }
    protected:
        PointerType m_Data = nullptr;
    };

    template <typename T, typename... Args>
    ScopedPointer<T> MakeScoped(Args&&... Arguments)
    {
        return ScopedPointer<T>(new T(std::forward<Args>(Arguments)...));
    }

    template <typename T, typename U> requires IsBaseOfValue<T, U>
    ScopedPointer<U> Cast(ScopedPointer<T>&& Other)
    {
        return ScopedPointer<U>(Other);
    }
}