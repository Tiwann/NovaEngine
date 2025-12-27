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

    Color::Color(uint32_t color)
    : r((color >> 24 & 0xFF) / 255.0f), g((color >> 16 & 0xFF) / 255.0f), b((color >> 8 & 0xFF) / 255.0f), a((color & 0xFF) / 255.0f)
    {
    }

    Color::Color(float red, float green, float blue, float alpha)
    : r(red), g(green), b(blue), a(alpha)
    {
    }
    
    Color::operator Vector4() const
    {
        return { r, g, b, a };
    }

    Color Color::FromHSL(float hue, float saturation, float lightness, float alpha)
    {
        return 0;
    }
    
    Color Color::WithHue(float hue)
    {
        return 0;
    }

    Color Color::WithSaturation(float saturation)
    {
        return 0;
    }

    Color Color::WithLightness(const float lightness)
    {
        const float l = Math::Clamp(lightness, 0.0f, 1.0f);
        return { r * l, g * l, b * l, a };
    }

    Color Color::WithOpacity(float opacity)
    {
        return { r, g, b, opacity };
    }

    Color Color::WithRed(float red)
    {
        return { red, g, b, a };
    }

    Color Color::WithGreen(float green)
    {
        return { r, green, b, a };
    }

    Color Color::WithBlue(float blue)
    {
        return { r, g, blue, a };
    }

    Color Color::Lerp(const Color a, const Color b, float t)
    {
        return {Math::Lerp(a.r, b.r, t), Math::Lerp(a.g, b.g, t), Math::Lerp(a.b, b.b, t), Math::Lerp(a.a, b.a, t)};
    }

    const Color Color::Red        { 0xFF0000FF };
    const Color Color::Green      { 0x00FF00FF };
    const Color Color::Blue       { 0x0000FFFF };
    const Color Color::Magenta    { 0xFF00FFFF };
    const Color Color::Cyan       { 0x00FFFFFF };
    const Color Color::Yellow     { 0xFFFF00FF };
    const Color Color::Black      { 0x000000FF };
    const Color Color::White      { 0xFFFFFFFF };
    const Color Color::Orange     { 0xFF8000FF };
    const Color Color::Pink       { 0xFFC0CBFF };
    const Color Color::Brown      { 0x00000000 };
    const Color Color::Purple     { 0x800080FF };
    const Color Color::Turquoise  { 0x40E0D0FF };
    const Color Color::Grey       { 0x808080FF };
}
