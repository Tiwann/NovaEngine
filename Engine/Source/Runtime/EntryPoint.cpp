#include "EntryPoint.h"
#include "Application.h"

namespace Nova
{
    int Main(const int Argc, char** Argv)
    {
        while (g_ApplicationRunning)
        {
            g_Application = CreateApplication(Array<const char*>(Argv, Argc));
            g_Application->Run();
            delete g_Application;
        }
        return g_ExitCode;
    }
}

#if defined(NOVA_PLATFORM_WINDOWS) && defined(NOVA_DIST)
#include <Windows.h>
INT APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, INT)
{
    return Nova::Main(__argc, __argv);
}
#else
int main(const int argc, char** argv)
{
    return Nova::Main(argc, argv);
}
#endif

