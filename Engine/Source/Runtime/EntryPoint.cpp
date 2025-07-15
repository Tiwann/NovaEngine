#include "Runtime/EntryPoint.h"


#if defined(NOVA_PLATFORM_WINDOWS)
#include <Windows.h>
#if defined(NOVA_RELEASE)
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
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
