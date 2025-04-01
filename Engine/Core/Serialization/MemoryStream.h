#pragma once
#include "Stream.h"
#include "Containers/Buffer.h"

namespace Nova
{
    class MemoryStream : public Stream
    {
    public:
        explicit MemoryStream(const BufferView<u8>& InBuffer);

        SizeType    Read(void* OutBuffer, SizeType Count) override;
        SizeType    Write(const void* InBuffer, SizeType Count) override;
        bool        Seek(Nova::Seek SeekMode, OffsetType Offset) override;
        OffsetType  Tell() const override;
        void        Close() override;
        bool        IsGood() const override;
    private:
        BufferView<u8> m_Buffer;
        OffsetType m_Position = 0;
    };
}
