#include "MemoryStream.h"

namespace Nova
{
    MemoryStream::MemoryStream(const BufferView<uint8_t>& buffer): Stream(OpenModeFlagBits::None), m_Buffer(buffer)
    {
        m_Opened = true;
    }

    Stream::SizeType MemoryStream::Read(void* outBuffer, const SizeType count)
    {
        if(!m_Opened) return EndOfFile;
        memcpy(outBuffer, &m_Buffer[m_Position], count);
        m_Position += (OffsetType)count;
        return count;
    }

    Stream::SizeType MemoryStream::Write(const void* inBuffer, const SizeType count)
    {
        if(!m_Opened) return EndOfFile;
        if(m_Position + count > m_Buffer.Count()) return EndOfFile;

        memcpy((void*)&m_Buffer[m_Position], inBuffer, count);
        m_Position += (OffsetType)count;
        return count;
    }

    bool MemoryStream::Seek(const Nova::Seek seek, const OffsetType offset)
    {
        if(!m_Opened) return false;
        switch (seek) {
        case Seek::Begin:
            if(offset < 0) return false;
            m_Position = offset;
            return true;
        case Seek::Current:
            if(m_Position + offset < 0) return false;
            m_Position += offset;
            return true;
        case Seek::End:
            if(offset > 0) return false;
            m_Position = (OffsetType)m_Buffer.Count() + offset;
            return true;
        }
        return false;
    }

    Stream::OffsetType MemoryStream::Tell() const
    {
        return m_Opened ? m_Position : EndOfFile;
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
