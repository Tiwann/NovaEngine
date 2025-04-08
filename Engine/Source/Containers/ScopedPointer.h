#pragma once

namespace Nova
{
    template <typename T>
    class ScopedPointer
    {
    public:
        using PointerType = T*;
        
        ScopedPointer(PointerType Obj) : m_Data(Obj) {}
        ~ScopedPointer()
        {
            delete m_Data;
        }

        operator PointerType() const  { return m_Data; }
        PointerType operator->() const { return m_Data; }
    private:
        PointerType m_Data = nullptr;
    };

    template <typename T, typename... Args>
    ScopedPointer<T> MakeScoped(Args&&... Arguments)
    {
        return ScopedPointer<T>(new T(std::forward<Args>(Arguments)...));
    }
}