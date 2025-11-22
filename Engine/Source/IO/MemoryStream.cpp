#include "MemoryStream.h"

namespace Nova
{
    MemoryStream::MemoryStream(const BufferView<uint8_t>& buffer): Stream(OpenModeFlagBits::None), m_Buffer(buffer)
    {
        m_Opened = true;
    }

    Stream::SizeType MemoryStream::ReadRaw(void* outBuffer, const SizeType size)
    {
        if(!m_Opened) return EndOfFile;
        memcpy(outBuffer, &m_Buffer[m_Position], size);
        m_Position += (OffsetType)size;
        return size;
    }

    Stream::SizeType MemoryStream::WriteRaw(const void* inBuffer, const SizeType size)
    {
        if(!m_Opened) return EndOfFile;
        if(m_Position + size > m_Buffer.Count()) return EndOfFile;

        memcpy((void*)&m_Buffer[m_Position], inBuffer, size);
        m_Position += (OffsetType)size;
        return size;
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
        return m_Opened ? m_Position : OffsetType(~0);
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
