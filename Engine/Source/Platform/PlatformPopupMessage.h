#pragma once
#if defined(NOVA_PLATFORM_WINDOWS)
#include "Windows/WindowsPopupMessage.h"
#elif defined(NOVA_PLATFORM_UNIX)
#include "Unix/UnixPopupMessage.h"
#endif