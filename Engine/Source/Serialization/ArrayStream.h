#pragma once
#include "Stream.h"

namespace Nova
{
    class ArrayStream : public Stream
    {
    public:
        ArrayStream();
        ~ArrayStream() override;

        ArrayStream(const ArrayStream&) = delete;
        ArrayStream& operator=(const ArrayStream&) = delete;
        ArrayStream(ArrayStream&&) = delete;
        ArrayStream& operator=(ArrayStream&&) = delete;

        bool IsGood() const override;
        SizeType Read(void* OutBuffer, SizeType Count) override;
        SizeType Write(const void* InBuffer, SizeType Count) override;
        bool Seek(Nova::Seek SeekMode, OffsetType Offset) override;
        OffsetType Tell() const override;

        BufferView<u8> GetView() const { return { m_Data, m_Count }; }
        
    private:
        u8* m_Data = nullptr;
        SizeType m_Count = 0;
        SizeType m_Allocated = 0;
        OffsetType m_Position = 0;
    };
}
