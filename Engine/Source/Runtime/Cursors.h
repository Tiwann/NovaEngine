#pragma once

namespace Nova
{
    class Window;
    
    enum class StandardCursors
    {
        Arrow,
        IBeam,
        Crosshair,
        Hand,
        HorizontalResize,
        VerticalResize,
    };
    
    class Cursor
    {
    public:
        static void SetCursorStandard(Window* Window, StandardCursors Cursors);
        static void SetCursorVisible(Window* Window, bool Visible);
        static void SetCursorLocked(Window* Window, bool Locked);
    private:
        static int StandardCursorConvert(StandardCursors Cursors);
    };

    
}
