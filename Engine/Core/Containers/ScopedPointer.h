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
}