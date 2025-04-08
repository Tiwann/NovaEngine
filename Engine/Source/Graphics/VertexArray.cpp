#include "VertexArray.h"


#include "Platform/PlatformVertexArray.h"

namespace Nova
{
    VertexArray* VertexArray::Create()
    {
        NOVA_RHI_PLATFORM_RETURN(VertexArray);
    }
}
