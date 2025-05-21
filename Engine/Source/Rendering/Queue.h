#pragma once
#include "RendererObject.h"

namespace Nova
{
    enum class QueueFamily
    {
        Graphics,
        Present,
        Compute,
        Transfer,
    };

    struct QueueRetrieveInfo
    {

    };

    class Queue : public RendererObject<QueueRetrieveInfo>
    {

    };
}
