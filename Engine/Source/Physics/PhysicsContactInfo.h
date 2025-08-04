#pragma once

namespace Nova
{
    template<typename BodyType, typename Vector> requires IsVectorValue<Vector>
    struct PhysicsContactInfo
    {
        Vector Point;
        Vector Normal;
        BodyType* OtherBody;
    };
}