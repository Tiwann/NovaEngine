#include "ArrayStream.h"

namespace Nova
{
    ArrayStream::ArrayStream() : Stream(OpenModeFlagBits::None)
    {
        m_Opened = true;
        m_Data = Memory::Malloc<u8>();
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

    Stream::SizeType ArrayStream::Read(void* OutBuffer, SizeType Count)
    {
        if(!m_Opened) return -1ULL;
        if (m_Position + Count > m_Count) return -1ULL;
        memcpy(OutBuffer, &m_Data[m_Position], Count);
        m_Position += (OffsetType)Count;
        return Count;
    }

    Stream::SizeType ArrayStream::Write(const void* InBuffer, SizeType Count)
    {
        if(!m_Opened) return -1ULL;
        while(m_Position + Count > m_Allocated)
        {
            m_Allocated *= 2ULL;
            u8* NewPlace = Memory::Malloc<u8>(m_Allocated);
            memcpy(NewPlace, m_Data, m_Count);
            Memory::Free(m_Data);
            m_Data = NewPlace;
        }

        memcpy(&m_Data[m_Position], InBuffer, Count);
        if (m_Position + Count > m_Count)
            m_Count += Count - (m_Count - m_Position);
        
        m_Position += (OffsetType)Count;
        return Count;
    }

    bool ArrayStream::Seek(Nova::Seek SeekMode, OffsetType Offset)
    {
        if(!m_Opened) return false;
        switch (SeekMode) {
        case Seek::Begin:
            if(Offset < 0) return false;
            if (Offset > (OffsetType)m_Count) return false;
            m_Position = Offset;
            return true;
        case Seek::Current:
            if(m_Position + Offset < 0) return false;
            if(m_Position + Offset > (OffsetType)m_Count) return false;
            m_Position += Offset;
            return true;
        case Seek::End:
            if(Offset > 0) return false;
            if((OffsetType)m_Count + Offset < 0) return false;
            m_Position = (OffsetType)m_Count + Offset;
            return true;
        }
        return false;
    }

    Stream::OffsetType ArrayStream::Tell() const
    {
        return m_Opened ? m_Position : -1L;
    }
}
