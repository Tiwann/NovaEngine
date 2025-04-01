#pragma once
#include "TypeTraits.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace Nova
{
    template<typename Vector> requires IsVectorValue<Vector>
    struct Ray
    {
        Vector3 Position;
        Vector3 Direction;
    };

    using Ray2D = Ray<Vector2>;
    using Ray3D = Ray<Vector3>;
}
