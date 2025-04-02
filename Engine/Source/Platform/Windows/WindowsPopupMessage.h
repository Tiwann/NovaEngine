#pragma once
#include "Core/PopupMessage.h"
#include "Core/Containers/Map.h"

namespace Hydro
{
    class WindowsPopupMessage : public PopupMessage
    {
    public:
        WindowsPopupMessage(NOVA_POPUPMESSAGE_SIGNATURE);
        PopupMessageResult Show() override;
        
        using ResponseConvertor = Map<PopupMessageResponse, i32>;
        using ResultConvertor = Map<i32, PopupMessageResult>;
        using IconConvertor = Map<PopupMessageIcon, i32>;
    private:
        static ResponseConvertor s_ResponseConvertor;
        static ResultConvertor s_ResultConvertor;
        static IconConvertor s_IconConvertor;
    };
}
