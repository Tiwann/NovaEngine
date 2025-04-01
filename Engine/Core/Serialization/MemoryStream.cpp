#include "MemoryStream.h"

namespace Nova
{
    MemoryStream::MemoryStream(const BufferView<u8>& InBuffer): Stream(OpenModeFlagBits::None), m_Buffer(InBuffer)
    {
        m_Opened = true;
    }

    Stream::SizeType MemoryStream::Read(void* OutBuffer, SizeType Count)
    {
        if(!m_Opened) return -1ULL;
        memcpy(OutBuffer, &m_Buffer[m_Position], Count);
        m_Position += (OffsetType)Count;
        return Count;
    }

    Stream::SizeType MemoryStream::Write(const void* InBuffer, SizeType Count)
    {
        if(!m_Opened) return -1ULL;
        if(m_Position + Count > m_Buffer.Count()) return -1ULL;
        memcpy((void*)&m_Buffer[m_Position], InBuffer, Count);
        m_Position += (OffsetType)Count;
        return Count;
    }

    bool MemoryStream::Seek(Nova::Seek SeekMode, OffsetType Offset)
    {
        if(!m_Opened) return false;
        switch (SeekMode) {
        case Seek::Begin:
            if(Offset < 0) return false;
            m_Position = Offset;
            return true;
        case Seek::Current:
            if(m_Position + Offset < 0) return false;
            m_Position += Offset;
            return true;
        case Seek::End:
            if(Offset > 0) return false;
            m_Position = (OffsetType)m_Buffer.Count() + Offset;
            return true;
        }
        return false;
    }

    Stream::OffsetType MemoryStream::Tell() const
    {
        return m_Opened ? m_Position : -1L;
    }

    void MemoryStream::Close()
    {
        Stream::Close();
    }

    bool MemoryStream::IsGood() const
    {
        return m_Position < (OffsetType)m_Buffer.Count();
    }
}
