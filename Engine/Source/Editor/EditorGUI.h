#pragma once
#include "Menu.h"
#include "Platform/PlatformImGui.h"
#include "Runtime/Flags.h"
#include "Containers/TreeNode.h"
#include "Runtime/Entity.h"
#include "Containers/ArrayView.h"
#include "Containers/StringView.h"
#include "Input/MouseButton.h"
#include "Math/LinearAlgebra.h"
#include "Containers/Function.h"


namespace Nova
{
    class Texture2D;
    
    enum class SliderFlagBits
    {
        None                = ImGuiSliderFlags_None,
        AlwaysClamp         = ImGuiSliderFlags_AlwaysClamp,
        Logarithmic         = ImGuiSliderFlags_Logarithmic,
        NoRoundToFormat     = ImGuiSliderFlags_NoRoundToFormat,
        NoInput             = ImGuiSliderFlags_NoInput,
    };
    NOVA_DECLARE_FLAGS(SliderFlagBits, SliderFlags)


    enum class ButtonFlagBits
    {
        None                   = 0,
        MouseButtonLeft        = 1 << 0,
        MouseButtonRight       = 1 << 1,
        MouseButtonMiddle      = 1 << 2,
    };
    NOVA_DECLARE_FLAGS(ButtonFlagBits, ButtonFlags);

    enum class WindowFlagBits
    {
        None                   = 0,
        NoTitleBar             = 1 << 0,   // Disable title-bar
        NoResize               = 1 << 1,   // Disable user resizing with the lower-right grip
        NoMove                 = 1 << 2,   // Disable user moving the window
        NoScrollbar            = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
        NoScrollWithMouse      = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
        NoCollapse             = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
        AlwaysAutoResize       = 1 << 6,   // Resize every window to its content every frame
        NoBackground           = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
        NoSavedSettings        = 1 << 8,   // Never load/save settings in .ini file
        NoMouseInputs          = 1 << 9,   // Disable catching mouse, hovering test with pass through.
        MenuBar                = 1 << 10,  // Has a menu-bar
        HorizontalScrollbar    = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
        NoFocusOnAppearing     = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
        NoBringToFrontOnFocus  = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
        AlwaysVerticalScrollbar= 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
        AlwaysHorizontalScrollbar=1<< 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
        NoNavInputs            = 1 << 16,  // No gamepad/keyboard navigation within the window
        NoNavFocus             = 1 << 17,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
        UnsavedDocument        = 1 << 18,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
        NoDocking              = 1 << 19,  // Disable docking of this window
        NoNav                  = NoNavInputs | NoNavFocus,
        NoDecoration           = NoTitleBar | NoResize | NoScrollbar | NoCollapse,
        NoInputs               = NoMouseInputs | NoNavInputs | NoNavFocus,
    };
    NOVA_DECLARE_FLAGS(WindowFlagBits, WindowFlags);
    
    enum class ColorFieldFlagBits
    {
        None            = 0,
        NoAlpha         = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only read 3 components from the input pointer).
        NoPicker        = 1 << 2,   //              // ColorEdit: disable picker when clicking on color square.
        NoOptions       = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
        NoSmallPreview  = 1 << 4,   //              // ColorEdit, ColorPicker: disable color square preview next to the inputs. (e.g. to show only the inputs)
        NoInputs        = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview color square).
        NoTooltip       = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
        NoLabel         = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
        NoSidePreview   = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small color square preview instead.
        NoDragDrop      = 1 << 9,   //              // ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.
        NoBorder        = 1 << 10,  //              // ColorButton: disable border (which is enforced by default)

        // User Options (right-click on widget to change some of them).
        AlphaBar        = 1 << 16,  //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
        AlphaPreview    = 1 << 17,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
        AlphaPreviewHalf= 1 << 18,  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
        HDR             = 1 << 19,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use Float flag as well).
        DisplayRGB      = 1 << 20,  // [Display]    // ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
        DisplayHSV      = 1 << 21,  // [Display]    // "
        DisplayHex      = 1 << 22,  // [Display]    // "
        Uint8           = 1 << 23,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
        Float           = 1 << 24,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
        PickerHueBar    = 1 << 25,  // [Picker]     // ColorPicker: bar for Hue, rectangle for Sat/Value.
        PickerHueWheel  = 1 << 26,  // [Picker]     // ColorPicker: wheel for Hue, triangle for Sat/Value.
        InputRGB        = 1 << 27,  // [Input]      // ColorEdit, ColorPicker: input and output data in RGB format.
        InputHSV        = 1 << 28,  // [Input]      // ColorEdit, ColorPicker: input and output data in HSV format.
    };

    NOVA_DECLARE_FLAGS(ColorFieldFlagBits, ColorFieldFlags);
    
    namespace UI
    {
        /* Window */
        void NewWindow(StringView Label, bool& Opened, WindowFlags Flags, const Function<void()>& Content);
        
        /* Text */
        void Text(const StringView Str);
        
        /* Sliders */
        template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
        bool SliderValue(std::string_view Label, T& Val, T Min = 0, T Max = 0, std::string_view Format = std::is_integral_v<T> ? "%d" : "%.3f", SliderFlags Flags = SliderFlagBits::None, bool Enabled = true)
        {
            bool Result = false;
            ImGui::BeginDisabled(!Enabled);
            if(std::is_integral_v<T>)
            {
                int Temp = (int)Val;
                Result = ImGui::SliderInt(Label.data(), &Temp, (int)Min, (int)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
                Val = (T)Temp;
            }

            if(std::is_floating_point_v<T>)
            {
                Result = ImGui::SliderFloat(Label.data(), (f32*)&Val, (f32)Min, (f32)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
            }
            ImGui::EndDisabled();
            return Result;
        }
        
        template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
        bool SliderVector2(std::string_view Label, Vector2& Vec, T Min = 0, T Max = 0, std::string_view Format = std::is_integral_v<T> ? "%d" : "%.3f", SliderFlags Flags = SliderFlagBits::None, bool Enabled = true)
        {
            bool Result = false;
            ImGui::BeginDisabled(!Enabled);
            if(std::is_integral_v<T>)
            {
                int Temp[2] = { (int)Vec.x, (int)Vec.y };
                Result = ImGui::SliderInt2(Label.data(), Temp, (int)Min, (int)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
                Vec.x = (T)Temp[0];
                Vec.y = (T)Temp[1];
            }

            if(std::is_floating_point_v<T>)
            {
                Result = ImGui::SliderFloat2(Label.data(), Vec.ValuePtr(), (f32)Min, (f32)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
            }
            ImGui::EndDisabled();
            return Result;
        }
        
        template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
        bool SliderVector3(std::string_view Label, Vector3& Vec, T Min = 0, T Max = 0, std::string_view Format = std::is_integral_v<T> ? "%d" : "%.3f", SliderFlags Flags = SliderFlagBits::None, bool Enabled = true)
        {
            bool Result = false;
            ImGui::BeginDisabled(!Enabled);
            if(std::is_integral_v<T>)
            {
                int Temp[3] = { (int)Vec.x, (int)Vec.y, (int)Vec.z };
                Result = ImGui::SliderInt3(Label.data(), Temp, (int)Min, (int)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
                Vec.x = (T)Temp[0];
                Vec.y = (T)Temp[1];
                Vec.z = (T)Temp[2];
            }

            if(std::is_floating_point_v<T>)
            {
                Result = ImGui::SliderFloat3(Label.data(), Vec.ValuePtr(), (f32)Min, (f32)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
            }
            ImGui::EndDisabled();
            return Result;
        }
        
        template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
        bool SliderVector4(std::string_view Label, Vector4& Vec, T Min = 0, T Max = 0, std::string_view Format = std::is_integral_v<T> ? "%d" : "%.3f", SliderFlags Flags = SliderFlagBits::None, bool Enabled = true)
        {
            bool Result = false;
            ImGui::BeginDisabled(!Enabled);
            if(std::is_integral_v<T>)
            {
                int Temp[4] = { (int)Vec.x, (int)Vec.y, (int)Vec.z, (int)Vec.w };
                Result = ImGui::SliderInt4(Label.data(), Temp, (int)Min, (int)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
                Vec.x = (T)Temp[0];
                Vec.y = (T)Temp[1];
                Vec.z = (T)Temp[2];
                Vec.w = (T)Temp[3];
            }

            if(std::is_floating_point_v<T>)
            {
                Result = ImGui::SliderFloat4(Label.data(), Vec.ValuePtr(), (f32)Min, (f32)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
            }
            ImGui::EndDisabled();
            return Result;
        }
        

        /* Drags */
        template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
        bool DragValue(std::string_view Label, T& Val, f32 Speed = 1, T Min = 0, T Max = 0, std::string_view Format = std::is_integral_v<T> ? "%d" : "%.3f", SliderFlags Flags = SliderFlagBits::None, bool Enabled = true)
        {
            bool Result = false;
            ImGui::BeginDisabled(!Enabled);
            if(std::is_integral_v<T>)
            {
                int Temp = (int)Val;
                Result = ImGui::DragInt(Label.data(), &Temp, Speed, (int)Min, (int)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
                Val = (T)Temp;
            }

            if(std::is_floating_point_v<T>)
            {
                Result = ImGui::DragFloat(Label.data(), (f32*)&Val, Speed, (f32)Min, (f32)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
            }
            ImGui::EndDisabled();
            return Result;
        }

        template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
        bool DragVector2(std::string_view Label, Vector2& Vec, f32 Speed = 1, T Min = 0, T Max = 0, std::string_view Format = std::is_integral_v<T> ? "%d" : "%.3f", SliderFlags Flags = SliderFlagBits::None, bool Enabled = true)
        {
            bool Result = false;
            ImGui::BeginDisabled(!Enabled);
            if(std::is_integral_v<T>)
            {
                int Temp[2] = { (int)Vec.x, (int)Vec.y };
                Result = ImGui::DragInt2(Label.data(), Temp, Speed, (int)Min, (int)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
                Vec.x = (T)Temp[0];
                Vec.y = (T)Temp[1];
            }

            if(std::is_floating_point_v<T>)
            {
                Result = ImGui::DragFloat2(Label.data(), Vec.ValuePtr(), Speed, (f32)Min, (f32)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
            }
            ImGui::EndDisabled();
            return Result;
        }

        template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
        bool DragVector3(std::string_view Label, Vector3& Vec, f32 Speed = 1, T Min = 0, T Max = 0, std::string_view Format = std::is_integral_v<T> ? "%d" : "%.3f", SliderFlags Flags = SliderFlagBits::None, bool Enabled = true)
        {
            bool Result = false;
            ImGui::BeginDisabled(!Enabled);
            if(std::is_integral_v<T>)
            {
                int Temp[3] = { (int)Vec.x, (int)Vec.y, (int)Vec.z };
                Result = ImGui::DragInt3(Label.data(), Temp, Speed, (int)Min, (int)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
                Vec.x = (T)Temp[0];
                Vec.y = (T)Temp[1];
                Vec.z = (T)Temp[2];
            }

            if(std::is_floating_point_v<T>)
            {
                Result = ImGui::DragFloat3(Label.data(), Vec.ValuePtr(), Speed, (f32)Min, (f32)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
            }
            ImGui::EndDisabled();
            return Result;
        }

        template<typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
        bool DragVector4(std::string_view Label, Vector4& Vec, f32 Speed = 1, T Min = 0, T Max = 0, std::string_view Format = std::is_integral_v<T> ? "%d" : "%.3f", SliderFlags Flags = SliderFlagBits::None, bool Enabled = true)
        {
            bool Result = false;
            ImGui::BeginDisabled(!Enabled);
            if(std::is_integral_v<T>)
            {
                int Temp[4] = { (int)Vec.x, (int)Vec.y, (int)Vec.z, (int)Vec.w };
                Result = ImGui::DragInt4(Label.data(), Temp, Speed, (int)Min, (int)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
                Vec.x = (T)Temp[0];
                Vec.y = (T)Temp[1];
                Vec.z = (T)Temp[2];
                Vec.w = (T)Temp[3];
                return Result;
            }

            if(std::is_floating_point_v<T>)
            {
                Result = ImGui::DragFloat4(Label.data(), Vec.ValuePtr(), Speed, (f32)Min, (f32)Max, Format.data(), Flags.As<ImGuiSliderFlags>());
            }
            ImGui::EndDisabled();
            return Result;
        }
        

        
        bool Button(const StringView Label, Vector2 Size = {0, 0}, bool Enabled = true);
        
        void MainMenuMenuBar(const TreeNode<MenuItem>& RootNode);
        
        void AddComponent(Entity* Entity);
        
        void Sprite(class Sprite& InOutSprite);

        /* Input */
        bool ItemClicked(MouseButton Button);

        bool Combo(StringView Name, int* Current, ArrayView<const char*> Names);

        void Image(const Texture2D* InTexture, const Vector2& DrawSize);
    }
    
}
