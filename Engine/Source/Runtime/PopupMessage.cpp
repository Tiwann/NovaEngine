#include "PopupMessage.h"
#include "Platform/PlatformPopupMessage.h"

namespace Nova
{
    PopupMessage::PopupMessage(const String& Title, const String& Message, PopupMessageResponse Response, PopupMessageIcon Icon)
        : Title(Title), Message(Message), Response(Response), Icon(Icon)
    {
        
    }

    PopupMessage::~PopupMessage()
    {
        OnPopupMessageClosed.ClearAll();
        OnPopupMessageOpened.ClearAll();
    }

    ScopedPointer<PopupMessage> PopupMessage::Create(const String& Title, const String& Message, PopupMessageResponse Response, PopupMessageIcon Icon)
    {
#if defined(NOVA_PLATFORM_WINDOWS)
        return MakeScoped<WindowsPopupMessage>(Title, Message, Response, Icon);
#else
        return MakeScoped<LinuxPopupMessage>(Title, Message, Response, Icon);
#endif
    }
}
