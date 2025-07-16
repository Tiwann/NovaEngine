#include "ArrayStream.h"

namespace Nova
{
    ArrayStream::ArrayStream() : Stream(OpenModeFlagBits::None)
    {
        m_Opened = true;
        m_Data = Memory::Malloc<uint8_t>();
        m_Allocated = 1;
        m_Count = 0;
    }

    ArrayStream::~ArrayStream()
    {
        Memory::Free(m_Data);
        m_Allocated = 0;
        m_Count = 0;
    }

    bool ArrayStream::IsGood() const
    {
        return true;
    }

    Stream::SizeType ArrayStream::Read(void* outBuffer, SizeType count)
    {
        if(!m_Opened) return -1ULL;
        if (m_Position + count > m_Count) return -1ULL;
        memcpy(outBuffer, &m_Data[m_Position], count);
        m_Position += (OffsetType)count;
        return count;
    }

    Stream::SizeType ArrayStream::Write(const void* inBuffer, SizeType count)
    {
        if(!m_Opened) return -1ULL;
        while(m_Position + count > m_Allocated)
        {
            m_Allocated *= 2ULL;
            uint8_t* NewPlace = Memory::Malloc<uint8_t>(m_Allocated);
            memcpy(NewPlace, m_Data, m_Count);
            Memory::Free(m_Data);
            m_Data = NewPlace;
        }

        memcpy(&m_Data[m_Position], inBuffer, count);
        if (m_Position + count > m_Count)
            m_Count += count - (m_Count - m_Position);
        
        m_Position += (OffsetType)count;
        return count;
    }

    bool ArrayStream::Seek(Nova::Seek seekMode, OffsetType offset)
    {
        if(!m_Opened) return false;
        switch (seekMode) {
        case Seek::Begin:
            if(offset < 0) return false;
            if (offset > (OffsetType)m_Count) return false;
            m_Position = offset;
            return true;
        case Seek::Current:
            if(m_Position + offset < 0) return false;
            if(m_Position + offset > (OffsetType)m_Count) return false;
            m_Position += offset;
            return true;
        case Seek::End:
            if(offset > 0) return false;
            if((OffsetType)m_Count + offset < 0) return false;
            m_Position = (OffsetType)m_Count + offset;
            return true;
        }
        return false;
    }

    Stream::OffsetType ArrayStream::Tell() const
    {
        return m_Opened ? m_Position : EndOfFile;
    }
}
