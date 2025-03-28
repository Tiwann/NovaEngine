#pragma once
#include "Containers/String.h"
#include "Logger.h"

#if defined(HYDRO_CORE)
    #if defined(HYDRO_DEBUG)
        #define HYDRO_LOG(CategoryName, InVerbosity, ...) \
        do { \
            Hydro::Logger& _Logger = Hydro::Log::GetCoreLogger(); \
            _Logger.SetCurrentCategory(CategoryName##LogCategory::s_CategoryName);\
            _Logger.Log(InVerbosity, __VA_ARGS__);\
        } \
        while(0)
    #else
        #define HYDRO_LOG(CategoryName, Verbosity, ...) HYDRO_VOID
    #endif
#else
    #if defined(HYDRO_DEBUG)
        #define HYDRO_LOG(CategoryName, InVerbosity, ...) \
        do { \
        Hydro::Logger& _Logger = Hydro::Log::GetClientLogger(); \
        _Logger.SetCurrentCategory(CategoryName##LogCategory::s_CategoryName);\
        _Logger.Log(InVerbosity, __VA_ARGS__);\
        } \
        while(0)
    #else
        #define HYDRO_LOG(CategoryName, Verbosity, ...)    HYDRO_VOID
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
