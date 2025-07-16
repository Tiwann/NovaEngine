layout(location = 0) in vec4 v_Position;
layout(location = 1) in vec2 v_TexCoords;
layout(location = 2) in vec3 v_Normal;
layout(location = 3) in vec4 v_Color;

layout(location = 4) out vec4 fragColor;

void main()
{
    fragColor = v_Color;
}