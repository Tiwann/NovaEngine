#pragma once

#ifdef NOVAENGINE_PHYSICS_SHARED
    #if defined(_WIN32) || defined(__CYGWIN__)
        #if defined(NOVAENGINE_PHYSICS_BUILD)
            #define NOVAENGINE_PHYSICS_API __declspec(dllexport)
        #else
            #define NOVAENGINE_PHYSICS_API __declspec(dllimport)
        #endif
        #define NOVAENGINE_PHYSICS_HIDDEN
    #elif defined(__GNUC__) && __GNUC__ >= 4
        #define NOVAENGINE_PHYSICS_API __attribute__((visibility("default")))
        #define NOVAENGINE_PHYSICS_HIDDEN __attribute__((visibility("hidden")))
    #else
        #error "Not supported on this platform!"
        #define NOVAENGINE_PHYSICS_API
        #define NOVAENGINE_PHYSICS_HIDDEN
    #endif
#else
    
    #define NOVAENGINE_PHYSICS_API
    #define NOVAENGINE_PHYSICS_HIDDEN
#endif