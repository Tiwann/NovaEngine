#pragma once
#include "Containers/Function.h"

namespace Nova
{
    struct Defer
    {
        using FunctionType = Function<void()>;
        Defer() = delete;
        Defer(FunctionType&& func) : m_Func(func) {}

        template<typename T>
        Defer(T* instance, FunctionType::MemberPointerType<T> member)
        {
            m_Func.BindMember(instance, member);
        }

        template<typename T>
        Defer(T& instance, FunctionType::MemberPointerType<T> member)
        {
            m_Func.BindMember(&instance, member);
        }

        ~Defer()
        {
            m_Func();
        }

    private:
        Function<void()> m_Func = nullptr;
    };
}
