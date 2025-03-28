#include <iostream>
#include "Audio.h"

int main()
{
    std::cout << "Hello World!\n";

    Sound sound;
    sound.LoadSound("sounds/sound.wav");
    return 0;
}
