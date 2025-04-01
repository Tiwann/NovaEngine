#pragma once

#define HYDRO_MOUSE_BUTTON_1         0
#define HYDRO_MOUSE_BUTTON_2         1
#define HYDRO_MOUSE_BUTTON_3         2
#define HYDRO_MOUSE_BUTTON_4         3
#define HYDRO_MOUSE_BUTTON_5         4
#define HYDRO_MOUSE_BUTTON_6         5
#define HYDRO_MOUSE_BUTTON_7         6
#define HYDRO_MOUSE_BUTTON_8         7
#define HYDRO_MOUSE_BUTTON_LAST      HYDRO_MOUSE_BUTTON_8
#define HYDRO_MOUSE_BUTTON_LEFT      HYDRO_MOUSE_BUTTON_1
#define HYDRO_MOUSE_BUTTON_RIGHT     HYDRO_MOUSE_BUTTON_2
#define HYDRO_MOUSE_BUTTON_MIDDLE    HYDRO_MOUSE_BUTTON_3

namespace Hydro
{
    enum class MouseButton
    {
        Left = HYDRO_MOUSE_BUTTON_LEFT,
        Right = HYDRO_MOUSE_BUTTON_RIGHT,
        Middle = HYDRO_MOUSE_BUTTON_MIDDLE,
        Button1 = HYDRO_MOUSE_BUTTON_1,
        Button2 = HYDRO_MOUSE_BUTTON_2,
        Button3 = HYDRO_MOUSE_BUTTON_3,
        Button4 = HYDRO_MOUSE_BUTTON_4,
        Button5 = HYDRO_MOUSE_BUTTON_5,
        Button6 = HYDRO_MOUSE_BUTTON_6,
        Button7 = HYDRO_MOUSE_BUTTON_7,
        Button8 = HYDRO_MOUSE_BUTTON_8,
    };
    
}
