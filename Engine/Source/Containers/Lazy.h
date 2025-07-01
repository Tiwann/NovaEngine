#pragma once
#include "Function.h"

namespace Nova
{
    template<typename T>
    class Lazy
    {
    public:

        Lazy() = default;
        explicit Lazy(const T& Value) : m_Value(Value) { }

        template<typename U = T> requires std::is_pointer<U>::value
        Lazy(std::nullptr_t) : m_Value(nullptr) { }

        const T& Get(const Function<T()>& GetFunc)
        {
            if (!m_IsDirty)
                return m_Value;

            m_Value = GetFunc();
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
    private:
        T m_Value;
        bool m_IsDirty = true;
    };
}
