#pragma once
#include "Containers/String.h"

namespace Nova
{
    template<typename T>
    static void Swap(T& A, T& B)
    {
        T& Temporary = A;
        A = B;
        B = Temporary;
    }

    String GetEnv(const String& Name);
}
