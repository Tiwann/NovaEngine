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
        SizeType Read(void* outBuffer, SizeType count) override;
        SizeType Write(const void* inBuffer, SizeType count) override;
        bool Seek(Nova::Seek seekMode, OffsetType offset) override;
        OffsetType Tell() const override;

        BufferView<uint8_t> GetView() const { return { m_Data, m_Count }; }
        
    private:
        uint8_t* m_Data = nullptr;
        SizeType m_Count = 0;
        SizeType m_Allocated = 0;
        OffsetType m_Position = 0;
    };
}
