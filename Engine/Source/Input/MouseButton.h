#pragma once

#define NOVA_MOUSE_BUTTON_1         0
#define NOVA_MOUSE_BUTTON_2         1
#define NOVA_MOUSE_BUTTON_3         2
#define NOVA_MOUSE_BUTTON_4         3
#define NOVA_MOUSE_BUTTON_5         4
#define NOVA_MOUSE_BUTTON_6         5
#define NOVA_MOUSE_BUTTON_7         6
#define NOVA_MOUSE_BUTTON_8         7
#define NOVA_MOUSE_BUTTON_LAST      NOVA_MOUSE_BUTTON_8
#define NOVA_MOUSE_BUTTON_LEFT      NOVA_MOUSE_BUTTON_1
#define NOVA_MOUSE_BUTTON_RIGHT     NOVA_MOUSE_BUTTON_2
#define NOVA_MOUSE_BUTTON_MIDDLE    NOVA_MOUSE_BUTTON_3

namespace Nova
{
    enum class MouseButton
    {
        Left = NOVA_MOUSE_BUTTON_LEFT,
        Right = NOVA_MOUSE_BUTTON_RIGHT,
        Middle = NOVA_MOUSE_BUTTON_MIDDLE,
        Button1 = NOVA_MOUSE_BUTTON_1,
        Button2 = NOVA_MOUSE_BUTTON_2,
        Button3 = NOVA_MOUSE_BUTTON_3,
        Button4 = NOVA_MOUSE_BUTTON_4,
        Button5 = NOVA_MOUSE_BUTTON_5,
        Button6 = NOVA_MOUSE_BUTTON_6,
        Button7 = NOVA_MOUSE_BUTTON_7,
        Button8 = NOVA_MOUSE_BUTTON_8,
    };
    
}
