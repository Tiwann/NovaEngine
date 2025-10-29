#define ARENA_IMPLEMENTATION
#ifdef NOVA_PLATFORM_WINDOWS
#define ARENA_BACKEND ARENA_BACKEND_WIN32_VIRTUALALLOC
#else
#ifdef NOVA_PLATFORM_LINUX
#define ARENA_BACKEND ARENA_BACKEND_LINUX_MMAP
#else
#error "Couldn't compile arena for this platform"
#endif
#endif
#include "arena.h"
