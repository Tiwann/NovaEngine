module Common;

public typealias vec2 = float2;
public typealias vec3 = float3;
public typealias vec4 = float4;
public typealias mat2 = float2x2;
public typealias mat3 = float3x3;
public typealias mat4 = float4x4;

public vec2 operator*(mat2 lhs, vec2 rhs)
{
    return mul(lhs, rhs);
}

public vec3 operator*(mat3 lhs, vec3 rhs)
{
    return mul(lhs, rhs);
}

public vec4 operator*(mat4 lhs, vec4 rhs)
{
    return mul(lhs, rhs);
}

public vec3 operator*(mat3 lhs, vec4 rhs)
{
    return mul(lhs, rhs.xyz);
}


public vec4 operator*(mat4 lhs, vec3 rhs)
{
    return mul(lhs, vec4(rhs, 1.0));
}

public vec4 operator*(mat4 lhs, vec2 rhs)
{
    return mul(lhs, vec4(rhs, 0.0, 1.0));
}

public vec3 operator*(mat3 lhs, vec2 rhs)
{
    return mul(lhs, vec3(rhs, 1.0));
}


public struct CameraData
{
    mat4 View;
    mat4 Projection;
}

public vec4 ConvertToClipSpace(const CameraData Camera, const mat4 Transform, vec4 Position)
{
    return Camera.Projection * Camera.View * Transform * Position;
}
