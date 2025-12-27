#pragma once

#include <cstdint>


namespace Nova
{
    struct Vector4;

    struct Color
    {
    public:
        float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

        Color();
        Color(uint32_t color);
        Color(float red, float green, float blue, float alpha);

        operator Vector4() const;

        static Color FromHSL(float hue, float saturation, float lightness, float alpha);
        
        Color WithHue(float hue);
        Color WithSaturation(float saturation);
        Color WithLightness(float lightness);
        Color WithOpacity(float opacity);
        Color WithRed(float red);
        Color WithGreen(float green);
        Color WithBlue(float blue);

        static Color Lerp(Color a, Color b, float t);

        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Magenta;
        static const Color Cyan;
        static const Color Yellow;
        static const Color Black;
        static const Color White;
        static const Color Orange;
        static const Color Pink;
        static const Color Brown;
        static const Color Purple;
        static const Color Turquoise;
        static const Color Grey;
    };
}
