#pragma once
#include "Runtime/Definitions.h"
#include <thread>

namespace Nova
{
    class Thread
    {
    public:
        template<typename Function, typename... Args>
        explicit Thread(Function&& InFunction, Args&&... InArgs) : m_Thread(InFunction, std::forward<Args>(InArgs)...)
        {

        }

        NOVA_FORCEINLINE void Detach();
        NOVA_FORCEINLINE void Join();
        NOVA_FORCEINLINE bool Joinable() const;

    private:
        std::thread m_Thread;
    };
}
