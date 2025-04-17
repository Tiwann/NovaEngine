#include "WindowsPopupMessage.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Containers/StringConversion.h"


namespace Nova
{
    WindowsPopupMessage::ResponseConvertor WindowsPopupMessage::s_ResponseConvertor
    {
        { PopupMessageResponse::OK, MB_OK },
        { PopupMessageResponse::OKCancel, MB_OKCANCEL },
        { PopupMessageResponse::YesNoCancel, MB_YESNOCANCEL },
        { PopupMessageResponse::YesNo, MB_YESNO },
        { PopupMessageResponse::RetryCancel, MB_RETRYCANCEL },
        { PopupMessageResponse::CancelTryContinue, MB_CANCELTRYCONTINUE },
        { PopupMessageResponse::AbortRetryIgnore, MB_ABORTRETRYIGNORE },
        { PopupMessageResponse::None, -1 },
    };
    
    WindowsPopupMessage::ResultConvertor WindowsPopupMessage::s_ResultConvertor
    {
        { IDOK, PopupMessageResult::OK },
        { IDCANCEL, PopupMessageResult::Cancel },
        { IDYES, PopupMessageResult::Yes },
        { IDNO, PopupMessageResult::No },
        { IDRETRY, PopupMessageResult::Retry },
        { IDTRYAGAIN, PopupMessageResult::Try },
        { IDCONTINUE, PopupMessageResult::Retry },
    };
    
    WindowsPopupMessage::IconConvertor WindowsPopupMessage::s_IconConvertor
    {
        { PopupMessageIcon::Error, MB_ICONERROR },
        { PopupMessageIcon::Question, MB_ICONQUESTION },
        { PopupMessageIcon::Warning, MB_ICONWARNING },
        { PopupMessageIcon::Info, MB_ICONINFORMATION }
    };
    
    WindowsPopupMessage::WindowsPopupMessage(const String& Title, const String& Message, PopupMessageResponse Response, PopupMessageIcon Icon) : PopupMessage(Title, Message, Response, Icon)
    {
    }

    PopupMessageResult WindowsPopupMessage::Show()
    {
        if(OnPopupMessageOpened.IsBound()) OnPopupMessageOpened.Broadcast();
        GLFWwindow* NativeWindow = g_Application->GetWindow()->GetNativeWindow();
        const HWND WindowHandle = glfwGetWin32Window(NativeWindow);

        const i32 Flags = s_ResponseConvertor[Response] | s_IconConvertor[Icon];
        const i32 Win32Result = MessageBox(WindowHandle, *StringConvertToWide(Message), *StringConvertToWide(Title), Flags);
        const PopupMessageResult Result = s_ResultConvertor[Win32Result];
        if(OnPopupMessageClosed.IsBound()) OnPopupMessageClosed.Broadcast();
        return Result;
    }
    
}
