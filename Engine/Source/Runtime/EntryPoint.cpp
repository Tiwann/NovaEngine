#include "Runtime/EntryPoint.h"

#ifndef NOVA_CUSTOM_ENTRY_POINT
#include "Runtime/Application.h"
namespace Nova
{
    Application* g_Application = nullptr;

    int GuardedMain(const int argc, char** argv)
    {
        g_Application = CreateApplication(argc, argv);
        g_Application->Run();
        delete g_Application;
        return 0;
    }
}
#endif

#if defined(NOVA_PLATFORM_WINDOWS) && defined(NOVA_RELEASE)
#include <Windows.h>
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
