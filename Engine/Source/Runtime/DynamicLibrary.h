#pragma once
#include "Filesystem.h"
#include "Containers/StringView.h"

namespace Nova
{
    class DynamicLibrary
    {
    public:
        explicit DynamicLibrary(const Path& Filepath);
        virtual ~DynamicLibrary() = default;

        bool Initialize();
        void Destroy() const;
        void* GetSymbol(const StringView& Name) const;

        template<typename FunctionPtrType> requires std::is_pointer_v<FunctionPtrType>
        FunctionPtrType GetSymbol(const StringView& Name)
        {
            return (FunctionPtrType)GetSymbol(Name);
        }
    private:
        Path m_Filepath;
        void* m_Handle = nullptr;
    };


}
