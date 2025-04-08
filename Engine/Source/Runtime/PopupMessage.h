#pragma once#include "Macros.h"
#include "Containers/MulticastDelegate.h"
#include "Containers/String.h"

#define NOVA_POPUPMESSAGE_SIGNATURE const String& Title, const String& Message, PopupMessageResponse Response, PopupMessageIcon Icon

namespace Nova
{
    enum class PopupMessageResponse
    {
        None,
        OK,
        OKCancel,
        YesNoCancel,
        YesNo,
        RetryCancel,
        CancelTryContinue,
        AbortRetryIgnore
    };

    enum class PopupMessageResult
    {
        None,
        OK,
        Cancel,
        Yes,
        No,
        Retry,
        Try,
        Continue,
        Abort,
        Ignore
    };

    enum class PopupMessageIcon
    {
        Error,
        Question,
        Warning,
        Info
    };

    
    
    class PopupMessage
    {
        using PopupMessageAction = MulticastDelegate<void()>;
    public:
        NOVA_NOT_COPYABLE_NOT_MOVABLE(PopupMessage);
        PopupMessage(NOVA_POPUPMESSAGE_SIGNATURE);
        virtual ~PopupMessage();
        
        String Title;
        String Message;
        PopupMessageResponse Response;
        PopupMessageIcon Icon;
        PopupMessageAction OnPopupMessageOpened;
        PopupMessageAction OnPopupMessageClosed;
        
        virtual PopupMessageResult Show() = 0;

        static PopupMessage* Create(NOVA_POPUPMESSAGE_SIGNATURE);
    };
}
