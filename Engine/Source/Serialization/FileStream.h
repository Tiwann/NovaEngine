#pragma once
#include "Stream.h"
#include <filesystem>
namespace Nova
{
    class FileStream : public Stream
    {
    public:
        FileStream() = default;
        explicit FileStream(std::filesystem::path filepath, OpenModeFlags openMode);
        
        bool Open(const std::filesystem::path& filepath, OpenModeFlags openMode);
        SizeType Read(void* outBuffer, SizeType count) override;
        SizeType Write(const void* inBuffer, SizeType count) override;
        bool Seek(Nova::Seek seekMode, OffsetType offset) override;
        OffsetType Tell() const override;
        void Close() override;
        OffsetType GetSize();
        bool IsGood() const override;
        

    private:
        std::filesystem::path m_Filepath;
        FILE* m_Handle = nullptr;
    };

    class StandardStream : public Stream
    {
    public:
        
        SizeType Read(void* outBuffer, SizeType count) override;
        SizeType Write(const void* inBuffer, SizeType count) override;
        bool Seek(Nova::Seek seekMode, OffsetType offset) override;
        OffsetType Tell() const override;
        void Close() override;
        bool IsGood() const override;
        
        static StandardStream StandardOut;
        static StandardStream StandardIn;
        static StandardStream StandardError;
        
    protected:
        explicit StandardStream(FILE* handle, const OpenModeFlags& openMode);
    
    private:
        FILE* m_Handle;
    };
}
