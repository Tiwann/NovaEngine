#include "Runtime/EntryPoint.h"

#ifndef NOVA_CUSTOM_ENTRY_POINT
#include "Runtime/Application.h"

namespace Nova
{
    Application* g_Application = nullptr;

    int GuardedMain(const int argc, char** arv)
    {
        g_Application = CreateApplication();
        g_Application->Run();
        delete g_Application;
        return 0;
    }
}
#endif

#if defined(NOVA_PLATFORM_WINDOWS)
#include <Windows.h>
#if defined(NOVA_RELEASE)
INT WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    return Nova::GuardedMain(__argc, __argv);
}
#else
int main(const int argc, char** argv)
{
    return Nova::GuardedMain(argc, argv);
}
#endif
#elif defined(NOVA_PLATFORM_LINUX)
#include <unistd.h>
int main(const int argc, char** argv)
{
    return Nova::GuardedMain(argc, argv);
}
#else
#error "Cannot build the engine on this platform!"
#endif
