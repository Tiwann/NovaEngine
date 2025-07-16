layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec4 a_Color;

layout(location = 0) out vec4 v_Position;
layout(location = 1) out vec2 v_TexCoords;
layout(location = 2) out vec3 v_Normal;
layout(location = 3) out vec4 v_Color;

void main()
{
    v_Position = vec4(a_Position, 1.0);
    gl_Position = v_Position;

    v_TexCoords = a_TexCoords;
    v_Normal = a_Normal;
    v_Color = a_Color;
}