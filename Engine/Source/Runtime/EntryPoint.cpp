#include "Runtime/EntryPoint.h"

#ifndef NOVA_CUSTOM_ENTRY_POINT
#include "Runtime/Application.h"
namespace Nova
{
    int GuardedMain(const int argc, char** argv)
    {
        Application* app = CreateApplication(argc, argv);
        app->Run();
        delete app;
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
