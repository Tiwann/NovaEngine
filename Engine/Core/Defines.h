#pragma once

#ifdef NOVAENGINE_CORE_SHARED
    #if defined(_WIN32) || defined(__CYGWIN__)
        #if defined(NOVAENGINE_CORE_BUILD)
            #define NOVAENGINE_CORE_API __declspec(dllexport)
        #else
            #define NOVAENGINE_CORE_API __declspec(dllimport)
        #endif
        #define NOVAENGINE_CORE_HIDDEN
    #elif defined(__GNUC__) && __GNUC__ >= 4
        #define NOVAENGINE_CORE_API __attribute__((visibility("default")))
        #define NOVAENGINE_CORE_HIDDEN __attribute__((visibility("hidden")))
    #else
        #error "Not supported on this platform!"
        #define NOVAENGINE_CORE_API
        #define NOVAENGINE_CORE_HIDDEN
    #endif
#else
    
    #define NOVAENGINE_CORE_API
    #define NOVAENGINE_CORE_HIDDEN
#endif