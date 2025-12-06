#include "ArrayStream.h"

namespace Nova
{
    ArrayStream::ArrayStream() : Stream(OpenModeFlagBits::None)
    {
        m_Opened = true;
        m_Data = Memory::Malloc<uint8_t>();
        m_Allocated = 1;
        m_Size = 0;
    }

    ArrayStream::~ArrayStream()
    {
        Memory::Free(m_Data);
        m_Allocated = 0;
        m_Size = 0;
    }

    bool ArrayStream::IsGood() const
    {
        return true;
    }

    Stream::SizeType ArrayStream::ReadRaw(void* outBuffer, SizeType size)
    {
        if(!m_Opened) return -1ULL;
        if (m_Position + size > m_Size) return -1ULL;
        memcpy(outBuffer, &m_Data[m_Position], size);
        m_Position += (OffsetType)size;
        return size;
    }

    Stream::SizeType ArrayStream::WriteRaw(const void* inBuffer, SizeType size)
    {
        if(!m_Opened) return -1ULL;
        while(m_Position + size > m_Allocated)
        {
            m_Allocated *= 2ULL;
            uint8_t* NewPlace = Memory::Malloc<uint8_t>(m_Allocated);
            memcpy(NewPlace, m_Data, m_Size);
            Memory::Free(m_Data);
            m_Data = NewPlace;
        }

        memcpy(&m_Data[m_Position], inBuffer, size);
        if (m_Position + size > m_Size)
            m_Size += size - (m_Size - m_Position);
        
        m_Position += (OffsetType)size;
        return size;
    }

    bool ArrayStream::Seek(Nova::Seek seekMode, OffsetType offset)
    {
        if(!m_Opened) return false;
        switch (seekMode) {
        case Seek::Begin:
            if(offset < 0) return false;
            if (offset > (OffsetType)m_Size) return false;
            m_Position = offset;
            return true;
        case Seek::Current:
            if(m_Position + offset < 0) return false;
            if(m_Position + offset > (OffsetType)m_Size) return false;
            m_Position += offset;
            return true;
        case Seek::End:
            if(offset > 0) return false;
            if((OffsetType)m_Size + offset < 0) return false;
            m_Position = (OffsetType)m_Size + offset;
            return true;
        }
        return false;
    }

    Stream::OffsetType ArrayStream::Tell() const
    {
        return m_Opened ? m_Position : (OffsetType)EndOfFile;
    }
}
