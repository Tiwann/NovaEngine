#include "Runtime/Utility.h"
#include "Containers/String.h"
#include <Windows.h>

namespace Nova
{
    String GetEnv(const String& Name)
    {
        char Buffer[256] = { 0 };
        const size_t Result = GetEnvironmentVariableA(*Name, Buffer, 256);
        return Result ? String { Buffer, Result } : String { };
    }
}