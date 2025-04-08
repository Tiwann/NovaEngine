#include "PopupMessage.h"
#include "Platform\PlatformPopupMessage.h"

namespace Nova
{
    PopupMessage::PopupMessage(NOVA_POPUPMESSAGE_SIGNATURE)
        : Title(Title), Message(Message), Response(Response), Icon(Icon)
    {
        
    }

    PopupMessage::~PopupMessage()
    {
        OnPopupMessageClosed.ClearAll();
        OnPopupMessageOpened.ClearAll();
    }

    PopupMessage* PopupMessage::Create(NOVA_POPUPMESSAGE_SIGNATURE)
    {
        NOVA_OS_PLATFORM_RETURN(PopupMessage, Title, Message, Response, Icon);
    }
}
