#include "Assertion.h"
#include "Log.h"
#include "Application.h"

namespace Nova
{
    void Assert(const bool Condition, const char* Message)
    {
        if (Condition == true) return;
        NOVA_LOG(Application, Verbosity::Error, "{}. File: {}. Line:{}", Message, String(__FILE__), __LINE__);
        NOVA_BREAK();
    }
}
