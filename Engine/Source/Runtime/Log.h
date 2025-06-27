#pragma once
#include "Containers/String.h"
#include "Logger.h"
#include <CoreExport.h>

#if defined(NOVA_CORE)
    #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        #define NOVA_LOG(CategoryName, InVerbosity, ...) \
        do { \
            Nova::Logger& _Logger = Nova::Log::GetCoreLogger(); \
            _Logger.SetCurrentCategory(CategoryName##LogCategory::s_CategoryName);\
            _Logger.Log(InVerbosity, __VA_ARGS__);\
        } \
        while(0)
    #else
        #define NOVA_LOG(CategoryName, Verbosity, ...)
    #endif
#else
    #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        #define NOVA_LOG(CategoryName, InVerbosity, ...) \
        do { \
        Nova::Logger& _Logger = Nova::Log::GetClientLogger(); \
        _Logger.SetCurrentCategory(CategoryName##LogCategory::s_CategoryName);\
        _Logger.Log(InVerbosity, __VA_ARGS__);\
        } \
        while(0)
    #else
        #define NOVA_LOG(CategoryName, Verbosity, ...)
    #endif
#endif

namespace Nova
{
    class CORE_API Log
    {
    public:
        static Logger& GetCoreLogger();
        static Logger& GetClientLogger();
        static void SetCurrentCategoryName(String CategoryName);
        friend class LogCategoryFormatter;
    private:
        static String s_CurrentCategoryName;
        static Logger s_CoreLogger;
        static Logger s_ClientLogger;
    };
}
