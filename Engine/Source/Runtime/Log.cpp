#include "Log.h"

namespace Nova
{
    Logger Log::s_CoreLogger = Logger("CORE");
    Logger Log::s_ClientLogger = Logger("CLIENT");
    
    String Log::s_CurrentCategoryName;
    
    Logger& Log::GetCoreLogger()
    {
        return s_CoreLogger;
    }

    Logger& Log::GetClientLogger()
    {
        return s_ClientLogger;
    }

    void Log::SetCurrentCategoryName(String CategoryName)
    {
        s_CurrentCategoryName = std::move(CategoryName);
    }
}
