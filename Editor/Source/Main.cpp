#include <iostream>

#include "../../Engine/Audio/Source/Audio.h"
#include "../../Engine/Core/Source/Core.h"


int main()
{
    std::cout << "Hello World!\n";
    Core core;
    core.Initialize();

    core.CreateMainWindow("Hello World", 600, 400);
    while (!core.MainWindowShouldClose())
    {
        core.PollEvents();
        core.MainWindowPresent();
    }

    core.Shutdown();
    return 0;
}
