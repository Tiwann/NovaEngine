#pragma once

#ifndef NOVA_ASSERT
#ifdef NOVA_USE_ASSERTION
#include <cassert>
#define NOVA_ASSERT(Condition, Message) assert(Condition && Message)
#else
#define NOVA_ASSERT(Condition, Message) Assert(Condition, Message)
#endif
#endif

#ifndef NOVA_BREAK
#ifdef NOVA_PLATFORM_WINDOWS
#define NOVA_BREAK() __debugbreak()
#else
#ifdef NOVA_PLATFORM_LINUX
#define NOVA_BREAK() __builtin_trap()
#else
#define NOVA_BREAK()
#endif
#endif
#endif

namespace Nova
{
    void Assert(bool Condition, const char* Message);
}