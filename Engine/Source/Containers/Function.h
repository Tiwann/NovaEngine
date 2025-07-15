#pragma once
#include <utility>
#include <memory>


namespace Nova
{
    template<typename Ret, typename... Args>
    class CallableBase
    {
    public:
        virtual ~CallableBase() = default;
        virtual Ret Invoke(Args&&... Arguments) = 0;
    };

    template<typename Function, typename Ret, typename... Args>
    class Callable : public CallableBase<Ret, Args...> {
    public:
        Callable(Function&& Func) : m_Func(std::forward<Function>(Func)) {}
    
        Ret Invoke(Args&&... Arguments) override
        {
            return m_Func(std::forward<Args>(Arguments)...);
        }
    private:
        Function m_Func;
    };

    template<typename Signature>
    class Function;

    template<typename Ret, typename... Args>
    class Function<Ret(Args...)>
    {
    public:
        using CallableBaseType = CallableBase<Ret, Args...>;
        using PointerType = Ret(*)(Args...);
        template<class Class> using MemberPointerType = Ret(Class::*)(Args...);
        
        Function() = default;
        Function(decltype(nullptr)&& Null) : m_Callable(Null) {}
        
        template<typename FunctionType>
        Function(FunctionType&& Func) : m_Callable(std::make_shared<Callable<FunctionType, Ret, Args...>>(std::forward<FunctionType>(Func))) {}

        template<class Class>
        Function(Class* Instance, MemberPointerType<Class> Member)
        {
            const auto Lambda = [Instance, Member](Args... Arguments) -> Ret
            {
                return (Instance->*Member)(std::forward<Args>(Arguments)...);
            };
            m_Callable = std::make_shared<Callable<decltype(Lambda), Ret, Args...>>(std::forward<decltype(Lambda)>(Lambda));
        }

        
        Ret Call(Args... Arguments) const
        {
            return m_Callable->Invoke(std::forward<Args>(Arguments)...);
        }

        Ret operator()(Args... Arguments) const
        {
            return m_Callable->Invoke(std::forward<Args>(Arguments)...);
        }

        template<class Class>
        void BindMember(Class* Instance, MemberPointerType<Class> Member)
        {
            const auto Lambda = [Instance, Member](Args... Arguments) -> Ret
            {
                return (Instance->*Member)(std::forward<Args>(Arguments)...);
            };
            m_Callable = std::make_shared<Callable<decltype(Lambda), Ret, Args...>>(std::forward<decltype(Lambda)>(Lambda));
        }

        operator bool() const { return (bool)m_Callable; }
        
        
    private:
        std::shared_ptr<CallableBaseType> m_Callable = nullptr;
    };
}
