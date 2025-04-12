#pragma vertex 
#version 460 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputTexCoords;
layout(location = 2) in vec3 inputNormal;
layout(location = 3) in vec4 inputColor;

layout(location = 0) out vec3 position;
layout(location = 1) out vec2 texCoords;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec4 color;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLocalToWorldNormal;

void main()
{
    vec4 newPosition = uProjection * uView * uModel * vec4(inputPosition, 1.0);
    position = newPosition.xyz;
    texCoords = inputTexCoords;
    normal = vec3(uLocalToWorldNormal * vec4(inputNormal, 0.0));
    color = inputColor;
    gl_Position = newPosition;
}

#pragma fragment
#version 460 core
#include "Globals.glsl"
#include "Lighting.glsl"

layout(location = 0) out vec3 position;
layout(location = 1) out vec2 texCoords;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec4 color;

out vec4 fragColor;

struct Material
{
    sampler2D   albedoTexture;
    sampler2D   specularTexture;
    sampler2D   glossinessTexture;
    sampler2D   emissiveTexture;
    sampler2D   normalTexture;
    
    vec4        albedoColor;
    float       specular;
    float       glossiness;
    vec4        emissiveColor;
};

uniform Material uMaterial;
uniform float uGlossiness;

void main()
{
    vec3 lightColor = uDirectionalLight.color.xyz * uDirectionalLight.intensity;
    vec3 n = normalize(Input.normal);
    vec3 l = normalize(uDirectionalLight.direction);
    float diffuse = saturate(dot(n, l));
    vec3 diffuseLight = diffuse * lightColor;
    vec3 ambientLight = uAmbientLight.color.xyz * uAmbientLight.intensity;
    
    vec3 v = normalize(uCameraViewDirection);
    vec3 h = normalize(l + v);
    float specular = saturate(dot(h, n));
    float glossiness = uGlossiness;
    float specularExp = exp2(glossiness * 11.0) + 2.0;
    specular = pow(specular, specularExp);
    
    vec3 finalColor = ambientLight + diffuseLight + specular;
    fragColor = vec4(finalColor, 1.0);
}