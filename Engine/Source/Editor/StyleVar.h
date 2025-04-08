#pragma once
#include <imgui.h>

namespace Nova::UI
{
    enum class StyleVar
    {
        Alpha,                    // f32     Alpha
        DisabledAlpha,            // f32     DisabledAlpha
        WindowPadding,            // ImVec2    WindowPadding
        WindowRounding,           // f32     WindowRounding
        WindowBorderSize,         // f32     WindowBorderSize
        WindowMinSize,            // ImVec2    WindowMinSize
        WindowTitleAlign,         // ImVec2    WindowTitleAlign
        ChildRounding,            // f32     ChildRounding
        ChildBorderSize,          // f32     ChildBorderSize
        PopupRounding,            // f32     PopupRounding
        PopupBorderSize,          // f32     PopupBorderSize
        FramePadding,             // ImVec2    FramePadding
        FrameRounding,            // f32     FrameRounding
        FrameBorderSize,          // f32     FrameBorderSize
        ItemSpacing,              // ImVec2    ItemSpacing
        ItemInnerSpacing,         // ImVec2    ItemInnerSpacing
        IndentSpacing,            // f32     IndentSpacing
        CellPadding,              // ImVec2    CellPadding
        ScrollbarSize,            // f32     ScrollbarSize
        ScrollbarRounding,        // f32     ScrollbarRounding
        GrabMinSize,              // f32     GrabMinSize
        GrabRounding,             // f32     GrabRounding
        TabRounding,              // f32     TabRounding
        TabBorderSize,            // f32     TabBorderSize
        TabBarBorderSize,         // f32     TabBarBorderSize
        TabBarOverlineSize,       // f32     TabBarOverlineSize
        TableAngledHeadersAngle,  // f32     TableAngledHeadersAngle
        TableAngledHeadersTextAlign,// ImVec2  TableAngledHeadersTextAlign
        ButtonTextAlign,          // ImVec2    ButtonTextAlign
        SelectableTextAlign,      // ImVec2    SelectableTextAlign
        SeparatorTextBorderSize,  // f32     SeparatorTextBorderSize
        SeparatorTextAlign,       // ImVec2    SeparatorTextAlign
        SeparatorTextPadding,     // ImVec2    SeparatorTextPadding
        DockingSeparatorSize,     // f32     DockingSeparatorSize
    };

    template<typename T>
    class ScopedStyleVarPusher
    {
    public:
        ScopedStyleVarPusher(StyleVar Var, T Value) : m_Variable(Var)
        {
            ImGui::PushStyleVar((ImGuiStyleVar)Var, Value);
        }

        ~ScopedStyleVarPusher()
        {
            ImGui::PopStyleVar();
        }
    private:
        StyleVar m_Variable;
    };
}
