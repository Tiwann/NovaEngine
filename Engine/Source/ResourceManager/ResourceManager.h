#pragma once
#include "Runtime/Filesystem.h"
#include "Containers/Map.h"
#include "Containers/String.h"


namespace Nova
{
    template<class T>
    class ResourceManager
    {
    protected:
        ResourceManager() = default;
    public:
        using MapType = Map<String, T*>;
        using PairType = typename MapType::PairType;
        
        virtual ~ResourceManager()
        {
            UnloadAll();
        }
        virtual T* Load(const String& Name, const Path& Filepath) = 0;
        virtual T* Retrieve(const String& Name) = 0;

        void UnloadAll()
        {
            for (Pair<String, T*>& Pair : m_Data)
            {
                delete Pair.Value;
                Pair.Value = nullptr;
            }
            m_Data.Clear();
        }
    protected:
        MapType m_Data;
    };
}
