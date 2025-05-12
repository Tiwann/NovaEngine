#pragma once
#include "CameraProjectionMode.h"
#include "Runtime/Types.h"

namespace Nova
{
    struct Vector2;
    
    struct CameraSettings
    {
        CameraProjectionMode ProjectionMode;
        f32 Width, Height;
        f32 FieldOfView;
        f32 OrthoSize;
        f32 NearPlane, FarPlane;

        static CameraSettings DefaultPerspective;
        static CameraSettings DefaultOrthographic;

        CameraSettings& WithDimension(f32 Width, f32 Height);
        CameraSettings& WithDimensions(const Vector2& Dimensions);
        CameraSettings& WithFOV(f32 FOV);
        CameraSettings& WithOrthoSize(f32 OrthographicSize);
        CameraSettings& SetType(CameraProjectionMode Type);
        CameraSettings& WithNearFarPlanes(f32 Near, f32 Far);
        void SetDimensions(Vector2 Size);
    };    
}
