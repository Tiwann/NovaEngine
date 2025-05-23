#pragma once

#define NOVA_KEY_UNKNOWN            -1
#define NOVA_KEY_SPACE              32
#define NOVA_KEY_APOSTROPHE         39
#define NOVA_KEY_COMMA              44
#define NOVA_KEY_MINUS              45
#define NOVA_KEY_PERIOD             46
#define NOVA_KEY_SLASH              47
#define NOVA_KEY_0                  48
#define NOVA_KEY_1                  49
#define NOVA_KEY_2                  50
#define NOVA_KEY_3                  51
#define NOVA_KEY_4                  52
#define NOVA_KEY_5                  53
#define NOVA_KEY_6                  54
#define NOVA_KEY_7                  55
#define NOVA_KEY_8                  56
#define NOVA_KEY_9                  57
#define NOVA_KEY_SEMICOLON          59
#define NOVA_KEY_EQUAL              61
#define NOVA_KEY_A                  65
#define NOVA_KEY_B                  66
#define NOVA_KEY_C                  67
#define NOVA_KEY_D                  68
#define NOVA_KEY_E                  69
#define NOVA_KEY_F                  70
#define NOVA_KEY_G                  71
#define NOVA_KEY_H                  72
#define NOVA_KEY_I                  73
#define NOVA_KEY_J                  74
#define NOVA_KEY_K                  75
#define NOVA_KEY_L                  76
#define NOVA_KEY_M                  77
#define NOVA_KEY_N                  78
#define NOVA_KEY_O                  79
#define NOVA_KEY_P                  80
#define NOVA_KEY_Q                  81
#define NOVA_KEY_R                  82
#define NOVA_KEY_S                  83
#define NOVA_KEY_T                  84
#define NOVA_KEY_U                  85
#define NOVA_KEY_V                  86
#define NOVA_KEY_W                  87
#define NOVA_KEY_X                  88
#define NOVA_KEY_Y                  89
#define NOVA_KEY_Z                  90
#define NOVA_KEY_LEFT_BRACKET       91
#define NOVA_KEY_BACKSLASH          92
#define NOVA_KEY_RIGHT_BRACKET      93
#define NOVA_KEY_GRAVE_ACCENT       96
#define NOVA_KEY_WORLD_1            161
#define NOVA_KEY_WORLD_2            162
#define NOVA_KEY_ESCAPE             256
#define NOVA_KEY_ENTER              257
#define NOVA_KEY_TAB                258
#define NOVA_KEY_BACKSPACE          259
#define NOVA_KEY_INSERT             260
#define NOVA_KEY_DELETE             261
#define NOVA_KEY_RIGHT              262
#define NOVA_KEY_LEFT               263
#define NOVA_KEY_DOWN               264
#define NOVA_KEY_UP                 265
#define NOVA_KEY_PAGE_UP            266
#define NOVA_KEY_PAGE_DOWN          267
#define NOVA_KEY_HOME               268
#define NOVA_KEY_END                269
#define NOVA_KEY_CAPS_LOCK          280
#define NOVA_KEY_SCROLL_LOCK        281
#define NOVA_KEY_NUM_LOCK           282
#define NOVA_KEY_PRINT_SCREEN       283
#define NOVA_KEY_PAUSE              284
#define NOVA_KEY_F1                 290
#define NOVA_KEY_F2                 291
#define NOVA_KEY_F3                 292
#define NOVA_KEY_F4                 293
#define NOVA_KEY_F5                 294
#define NOVA_KEY_F6                 295
#define NOVA_KEY_F7                 296
#define NOVA_KEY_F8                 297
#define NOVA_KEY_F9                 298
#define NOVA_KEY_F10                299
#define NOVA_KEY_F11                300
#define NOVA_KEY_F12                301
#define NOVA_KEY_F13                302
#define NOVA_KEY_F14                303
#define NOVA_KEY_F15                304
#define NOVA_KEY_F16                305
#define NOVA_KEY_F17                306
#define NOVA_KEY_F18                307
#define NOVA_KEY_F19                308
#define NOVA_KEY_F20                309
#define NOVA_KEY_F21                310
#define NOVA_KEY_F22                311
#define NOVA_KEY_F23                312
#define NOVA_KEY_F24                313
#define NOVA_KEY_F25                314
#define NOVA_KEY_KP_0               320
#define NOVA_KEY_KP_1               321
#define NOVA_KEY_KP_2               322
#define NOVA_KEY_KP_3               323
#define NOVA_KEY_KP_4               324
#define NOVA_KEY_KP_5               325
#define NOVA_KEY_KP_6               326
#define NOVA_KEY_KP_7               327
#define NOVA_KEY_KP_8               328
#define NOVA_KEY_KP_9               329
#define NOVA_KEY_KP_DECIMAL         330
#define NOVA_KEY_KP_DIVIDE          331
#define NOVA_KEY_KP_MULTIPLY        332
#define NOVA_KEY_KP_SUBTRACT        333
#define NOVA_KEY_KP_ADD             334
#define NOVA_KEY_KP_ENTER           335
#define NOVA_KEY_KP_EQUAL           336
#define NOVA_KEY_LEFT_SHIFT         340
#define NOVA_KEY_LEFT_CONTROL       341
#define NOVA_KEY_LEFT_ALT           342
#define NOVA_KEY_LEFT_SUPER         343
#define NOVA_KEY_RIGHT_SHIFT        344
#define NOVA_KEY_RIGHT_CONTROL      345
#define NOVA_KEY_RIGHT_ALT          346
#define NOVA_KEY_RIGHT_SUPER        347
#define NOVA_KEY_MENU               348
#define NOVA_KEY_TOTAL              NOVA_KEY_MENU

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4309)
#endif
namespace Nova
{
    enum class KeyCode
    {
        Unknown = NOVA_KEY_UNKNOWN,
        Space = NOVA_KEY_SPACE,
        Apostrophe = NOVA_KEY_APOSTROPHE,
        Comma = NOVA_KEY_COMMA,
        Minus = NOVA_KEY_MINUS,
        Period = NOVA_KEY_PERIOD,
        Slash = NOVA_KEY_SLASH,
        Key0 = NOVA_KEY_0,
        Key1 = NOVA_KEY_1,
        Key2 = NOVA_KEY_2,
        Key3 = NOVA_KEY_3,
        Key4 = NOVA_KEY_4,
        Key5 = NOVA_KEY_5,
        Key6 = NOVA_KEY_6,
        Key7 = NOVA_KEY_7,
        Key8 = NOVA_KEY_8,
        Key9 = NOVA_KEY_9,
        SemiColon = NOVA_KEY_SEMICOLON,
        Equal = NOVA_KEY_EQUAL,
        KeyA = NOVA_KEY_A,
        KeyB = NOVA_KEY_B,
        KeyC = NOVA_KEY_C,
        KeyD = NOVA_KEY_D,
        KeyE = NOVA_KEY_E,
        KeyF = NOVA_KEY_F,
        KeyG = NOVA_KEY_G,
        KeyH = NOVA_KEY_H,
        KeyI = NOVA_KEY_I,
        KeyJ = NOVA_KEY_J,
        KeyK = NOVA_KEY_K,
        KeyL = NOVA_KEY_L,
        KeyM = NOVA_KEY_M,
        KeyN = NOVA_KEY_N,
        KeyO = NOVA_KEY_O,
        KeyP = NOVA_KEY_P,
        KeyQ = NOVA_KEY_Q,
        KeyR = NOVA_KEY_R,
        KeyS = NOVA_KEY_S,
        KeyT = NOVA_KEY_T,
        KeyU = NOVA_KEY_U,
        KeyV = NOVA_KEY_V,
        KeyW = NOVA_KEY_W,
        KeyX = NOVA_KEY_X,
        KeyY = NOVA_KEY_Y,
        KeyZ = NOVA_KEY_Z,
        LeftBracket = NOVA_KEY_LEFT_BRACKET,
        BackSlash = NOVA_KEY_BACKSLASH,
        RightBracket = NOVA_KEY_RIGHT_BRACKET,
        GraveAccent = NOVA_KEY_GRAVE_ACCENT,
        World1 = NOVA_KEY_WORLD_1,
        World2 = NOVA_KEY_WORLD_2,
        Escape = NOVA_KEY_ESCAPE,
        Enter = NOVA_KEY_ENTER,
        Tab = NOVA_KEY_TAB,
        Backspace = NOVA_KEY_BACKSPACE,
        Insert = NOVA_KEY_INSERT,
        Delete = NOVA_KEY_DELETE,
        Right = NOVA_KEY_RIGHT,
        Left = NOVA_KEY_LEFT,
        Down = NOVA_KEY_DOWN,
        Up = NOVA_KEY_UP,
        PageUp = NOVA_KEY_PAGE_UP,
        PageDown = NOVA_KEY_PAGE_DOWN,
        Home = NOVA_KEY_HOME,
        End = NOVA_KEY_END,
        CapsLock = NOVA_KEY_CAPS_LOCK,
        ScrollLock = NOVA_KEY_SCROLL_LOCK,
        NumLock = NOVA_KEY_NUM_LOCK,
        PrintScreen = NOVA_KEY_PRINT_SCREEN,
        Pause = NOVA_KEY_PAUSE,
        F1 = NOVA_KEY_F1,
        F2 = NOVA_KEY_F2,
        F3 = NOVA_KEY_F3,
        F4 = NOVA_KEY_F4,
        F5 = NOVA_KEY_F5,
        F6 = NOVA_KEY_F6,
        F7 = NOVA_KEY_F7,
        F8 = NOVA_KEY_F8,
        F9 = NOVA_KEY_F9,
        F10 = NOVA_KEY_F10,
        F11 = NOVA_KEY_F11,
        F12 = NOVA_KEY_F12,
        F13 = NOVA_KEY_F13,
        F14 = NOVA_KEY_F14,
        F15 = NOVA_KEY_F15,
        F16 = NOVA_KEY_F16,
        F17 = NOVA_KEY_F17,
        F18 = NOVA_KEY_F18,
        F19 = NOVA_KEY_F19,
        F20 = NOVA_KEY_F20,
        F21 = NOVA_KEY_F21,
        F22 = NOVA_KEY_F22,
        F23 = NOVA_KEY_F23,
        F24 = NOVA_KEY_F24,
        F25 = NOVA_KEY_F25,
        Numpad0 = NOVA_KEY_KP_0,
        Numpad1 = NOVA_KEY_KP_1,
        Numpad2 = NOVA_KEY_KP_2,
        Numpad3 = NOVA_KEY_KP_3,
        Numpad4 = NOVA_KEY_KP_4,
        Numpad5 = NOVA_KEY_KP_5,
        Numpad6 = NOVA_KEY_KP_6,
        Numpad7 = NOVA_KEY_KP_7,
        Numpad8 = NOVA_KEY_KP_8,
        Numpad9 = NOVA_KEY_KP_9,
        NumpadDecimal = NOVA_KEY_KP_DECIMAL,
        NumpadDivide = NOVA_KEY_KP_DIVIDE,
        NumpadMultiply = NOVA_KEY_KP_MULTIPLY,
        NumpadSubtract = NOVA_KEY_KP_SUBTRACT,
        NumpadAdd = NOVA_KEY_KP_ADD,
        NumpadEnter = NOVA_KEY_KP_ENTER,
        NumpadEqual = NOVA_KEY_KP_EQUAL,
        LeftShift = NOVA_KEY_LEFT_SHIFT,
        LeftControl = NOVA_KEY_LEFT_CONTROL,
        LeftAlt = NOVA_KEY_LEFT_ALT,
        LeftSuper = NOVA_KEY_LEFT_SUPER,
        RightShift = NOVA_KEY_RIGHT_SHIFT,
        RightControl = NOVA_KEY_RIGHT_CONTROL,
        RightAlt = NOVA_KEY_RIGHT_ALT,
        RightSuper = NOVA_KEY_RIGHT_SUPER,
        Menu = NOVA_KEY_MENU,
        Total = Menu
    };
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif