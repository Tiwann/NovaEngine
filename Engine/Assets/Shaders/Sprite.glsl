#pragma vertex
#version 460 core

layout(location = 0, binding = 0) in vec3 vPosition;
layout(location = 1, binding = 0) in vec2 vTexCoord;
layout(location = 2, binding = 0) in vec3 vNormal;
layout(location = 3, binding = 0) in vec4 vColor;


layout(location = 0, binding = 0) out vec3 oPosition;
layout(location = 1, binding = 0) out vec2 oTexCoord;
layout(location = 2, binding = 0) out vec3 oNormal;
layout(location = 3, binding = 0) out vec4 oColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uSpriteScale;
uniform vec2 uTiling;

void main()
{
    vec4 newPosition = uProjection * uView * uModel * vec4(uSpriteScale * vPosition, 1.0);
    oPosition = newPosition.xyz;
    oTexCoord = vTexCoord * uTiling;
    oNormal = vNormal;
    oColor = vColor;
    gl_Position = newPosition;
}

#pragma fragment
#version 460 core

layout(location = 0) in vec3 oPosition;
layout(location = 1) in vec2 oTexCoord;
layout(location = 2) in vec3 oNormal;
layout(location = 3) in vec4 oColor;

uniform sampler2D uTexture;
uniform vec4 uColorTint;

layout(location = 5) out vec4 fragColor;

void main()
{
    vec4 colorTexture = texture(uTexture, oTexCoord);
    if(colorTexture.a <= 0.0001)
        discard;
    fragColor = colorTexture * uColorTint;
}