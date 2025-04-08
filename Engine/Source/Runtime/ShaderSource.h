#pragma once
#include "Containers/String.h"
#include <optional>

namespace Nova
{
    struct ShaderSource
    {
        String Vertex;
        String Fragment;
        struct Define
        {
            String Key; 
            std::optional<String> Value;
        };
        Array<Define> Defines;

        void OnSources(const Function<void(const String& Source)>& Callback) const
        {
            Callback(Vertex);
            Callback(Fragment);
        }

        void OnSources(const Function<void(String& Source)>& Callback)
        {
            Callback(Vertex);
            Callback(Fragment);
        }
    };
}
