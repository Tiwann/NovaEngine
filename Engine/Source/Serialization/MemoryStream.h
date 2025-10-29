#pragma once
#include "Stream.h"
#include "Containers/BufferView.h"

namespace Nova
{
    class MemoryStream : public Stream
    {
    public:
        explicit MemoryStream(const BufferView<uint8_t>& buffer);

        SizeType    Read(void* outBuffer, SizeType size) override;
        SizeType    Write(const void* inBuffer, SizeType count) override;
        bool        Seek(Nova::Seek seek, OffsetType offset) override;
        OffsetType  Tell() const override;
        void        Close() override;
        bool        IsGood() const override;
    private:
        BufferView<uint8_t> m_Buffer;
        OffsetType m_Position = 0;
    };
}
