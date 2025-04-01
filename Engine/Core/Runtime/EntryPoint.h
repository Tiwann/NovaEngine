#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Types.h"

#define NOVA_DEFINE_APPLICATION_CLASS(ApplicationClass) Nova::Application* Nova::CreateApplication(const Nova::Array<const char*>& Arguments) \
    { \
        static_assert(Nova::IsBaseOfValue<::Nova::Application, ApplicationClass>, #ApplicationClass" isn't derived from Application");\
        return new (ApplicationClass)(Arguments); \
    } \

namespace Nova
{
    extern class Application* CreateApplication(const Array<const char*>& Arguments);
    inline bool g_ApplicationRunning = true;
    inline i32 g_ExitCode = 0;
    int Main(int Argc, char** Argv);
}

