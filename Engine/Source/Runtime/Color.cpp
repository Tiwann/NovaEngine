#include "Color.h"

#include "Math/Functions.h"
#include "Math/Vector4.h"

namespace Nova
{
    Color::Color()
    {
        r = g = b = 0.0f;
        a = 1.0f;
    }

    Color::Color(u32 color)
    : r((color >> 24 & 0xFF) / 255.0f), g((color >> 16 & 0xFF) / 255.0f), b((color >> 8 & 0xFF) / 255.0f), a((color & 0xFF) / 255.0f)
    {
    }

    Color::Color(f32 red, f32 green, f32 blue, f32 alpha)
    : r(red), g(green), b(blue), a(alpha)
    {
    }
    
    Color::operator Vector4() const
    {
        return { r, g, b, a };
    }

    Color Color::FromHSL(f32 hue, f32 saturation, f32 lightness, f32 alpha)
    {
        return 0;
    }
    
    Color Color::WithHue(f32 hue)
    {
        return 0;
    }

    Color Color::WithSaturation(f32 saturation)
    {
        return 0;
    }

    Color Color::WithLightness(const f32 lightness)
    {
        const f32 l = Math::Clamp(lightness, 0.0f, 1.0f);
        return { r * l, g * l, b * l, a };
    }

    Color Color::WithOpacity(f32 opacity)
    {
        return { r, g, b, opacity };
    }

    Color Color::WithRed(f32 red)
    {
        return { red, g, b, a };
    }

    Color Color::WithGreen(f32 green)
    {
        return { r, green, b, a };
    }

    Color Color::WithBlue(f32 blue)
    {
        return { r, g, blue, a };
    }

    Color Color::Lerp(Color A, Color B, f32 Alpha)
    {
        return {Math::Lerp(A.r, B.r, Alpha), Math::Lerp(A.g, B.g, Alpha), Math::Lerp(A.b, B.b, Alpha), Math::Lerp(A.a, B.a, Alpha)};
    }

    Color Color::Red        { 0xFF0000FF };
    Color Color::Green      { 0x00FF00FF };
    Color Color::Blue       { 0x0000FFFF };
    Color Color::Magenta    { 0xFF00FFFF };
    Color Color::Cyan       { 0x00FFFFFF };
    Color Color::Yellow     { 0xFFFF00FF };
    Color Color::Black      { 0x000000FF };
    Color Color::White      { 0xFFFFFFFF };
    Color Color::Orange     { 0xFF8000FF };
    Color Color::Pink       { 0xFFC0CBFF };
    Color Color::Brown      { 0x00000000 };
    Color Color::Purple     { 0x800080FF };
    Color Color::Turquoise  { 0x40E0D0FF };
    Color Color::Grey       { 0x808080FF };
}
