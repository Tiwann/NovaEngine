#pragma once
#include "Containers/MulticastDelegate.h"
#include "Containers/ScopedPointer.h"
#include "Containers/String.h"

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
        PopupMessage(const String& Title, const String& Message, PopupMessageResponse Response, PopupMessageIcon Icon);
        virtual ~PopupMessage();

        String Title;
        String Message;
        PopupMessageResponse Response;
        PopupMessageIcon Icon;
        PopupMessageAction OnPopupMessageOpened;
        PopupMessageAction OnPopupMessageClosed;
        
        virtual PopupMessageResult Show() = 0;

        static ScopedPointer<PopupMessage> Create(const String& Title, const String& Message, PopupMessageResponse Response, PopupMessageIcon Icon);
    };
}
