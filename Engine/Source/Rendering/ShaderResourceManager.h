#pragma once
#include "Runtime/Object.h"

namespace Nova
{
    class Shader;

    class ShaderResourceManager : public Object
    {
    public:
        explicit ShaderResourceManager(Shader* Shader);

    private:
        Shader* m_Shader = nullptr;
    };
}
