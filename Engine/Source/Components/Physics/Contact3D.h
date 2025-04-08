#pragma once
#include "Math/Vector3.h"


namespace Nova
{
    class RigidBody3D;
    
    struct Contact3D
    {
        Vector3 ImpactPoint;
        Vector3 Normal;
        RigidBody3D* Other;
    };
}
