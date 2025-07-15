#pragma once

#if !defined(NOVA_API)
    #if defined(NOVA_PLATFORM_WINDOWS)
        #define NOVA_EXPORT __declspec(dllexport)
        #define NOVA_IMPORT __declspec(dllimport)
    #elif defined(NOVA_PLATFORM_LINUX)
        #define NOVA_EXPORT __attribute__((visibility("default")))
        #define NOVA_IMPORT __attribute__((visibility("default")))
    #endif

    #if defined(NOVA_BUILD_SHARED)
        #if defined(NOVA_CORE)
            #define NOVA_API NOVA_EXPORT
        #else
            #define NOVA_API NOVA_IMPORT
        #endif
    #else
        #define NOVA_API
    #endif
#endif