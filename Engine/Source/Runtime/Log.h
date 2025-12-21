#pragma once
#include "Containers/String.h"
#include "Logger.h"

#if defined(NOVA_CORE)
    #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        #define NOVA_LOG(category, verbosity, ...) \
        do { \
            Nova::Logger& _Logger = Nova::Log::GetCoreLogger(); \
            _Logger.SetCurrentCategory(category##LogCategory::s_CategoryName);\
            _Logger.Log(verbosity, __VA_ARGS__);\
        } \
        while(0)
    #else
        #define NOVA_LOG(category, verbosity, ...)
    #endif
#else
    #if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        #define NOVA_LOG(category, verbosity, ...) \
        do { \
        Nova::Logger& _Logger = Nova::Log::GetClientLogger(); \
        _Logger.SetCurrentCategory(category##LogCategory::s_CategoryName);\
        _Logger.Log(verbosity, __VA_ARGS__);\
        } \
        while(0)
    #else
        #define NOVA_LOG(category, verbosity, ...)
    #endif
#endif

namespace Nova
{
    class Log
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
