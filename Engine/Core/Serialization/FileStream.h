#pragma once
#include "Stream.h"

namespace Nova
{
    class FileStream : public Stream
    {
    public:
        explicit FileStream(Path Filepath, OpenModeFlags OpenMode);
        
        bool Open(const Path& Filepath, OpenModeFlags OpenMode);
        SizeType Read(void* OutBuffer, SizeType Count) override;
        SizeType Write(const void* InBuffer, SizeType Count) override;
        bool Seek(Nova::Seek SeekMode, OffsetType Offset) override;
        OffsetType Tell() const override;
        void Close() override;
        OffsetType GetSize();
        bool IsGood() const override;
        

    private:
        Path m_Filepath;
        FILE* m_Handle = nullptr;
    };

    class StandardStream : public Stream
    {
    public:
        
        SizeType Read(void* OutBuffer, SizeType Count) override;
        SizeType Write(const void* InBuffer, SizeType Count) override;
        bool Seek(Nova::Seek SeekMode, OffsetType Offset) override;
        OffsetType Tell() const override;
        void Close() override;
        bool IsGood() const override;
        
        static StandardStream StandardOut;
        static StandardStream StandardIn;
        static StandardStream StandardError;
        
    protected:
        explicit StandardStream(FILE* Handle, const OpenModeFlags& OpenMode);
    
    private:
        FILE* m_Handle;
    };
}
