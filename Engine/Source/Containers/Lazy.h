#pragma once
#include "Function.h"

namespace Nova
{
    template<typename T>
    class Lazy
    {
    public:

        Lazy() = default;
        Lazy(const T& value) : m_Value(value) { }

        const T& Get(const Function<T()>& getter)
        {
            if (!m_IsDirty)
                return m_Value;

            m_Value = getter();
            m_IsDirty = false;
            return m_Value;
        }

        void SetDirty()
        {
            m_IsDirty = true;
        }

        bool IsDirty() const
        {
            return m_IsDirty;
        }

        T& operator*() { return m_Value; }
        const T& operator*() const { return m_Value; }

        T* operator&() { return &m_Value;}
        const T* operator&() const { return &m_Value; }

    private:
        T m_Value;
        bool m_IsDirty = true;
    };
}
