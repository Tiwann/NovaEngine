#pragma once

#ifndef @MODULE_NAME@_API
#ifdef MODULE_SHARED
    #if defined(_WIN32) || defined(__CYGWIN__)
        #if defined(MODULE_BUILD)
            #define @MODULE_NAME@_API __declspec(dllexport)
        #else
            #define @MODULE_NAME@_API __declspec(dllimport)
        #endif
        #define @MODULE_NAME@_HIDDEN
    #elif defined(__GNUC__) && __GNUC__ >= 4
        #define @MODULE_NAME@_API __attribute__((visibility("default")))
        #define @MODULE_NAME@_HIDDEN __attribute__((visibility("hidden")))
    #else
        #error "Not supported on this platform!"
        #define @MODULE_NAME@_API
        #define @MODULE_NAME@_HIDDEN
    #endif
#else
    #define @MODULE_NAME@_API
    #define @MODULE_NAME@_HIDDEN
#endif
#endif