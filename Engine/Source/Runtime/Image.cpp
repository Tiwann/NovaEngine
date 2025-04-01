#include "Image.h"
#include "Formats.h"
#include <stb/stb_image.h>

namespace Nova
{
    Image::Image(const u32 Width, const u32 Height, const Format Format, const void* Pixels) : m_Width(Width), m_Height(Height), m_Format(Format)
    {
        const size_t BytesPerPixel = GetBytesPerPixel(Format);
        const size_t TotalBytes = Width * Height * BytesPerPixel;
        m_Pixels = malloc(TotalBytes);
        std::copy_n((const u8*)Pixels, TotalBytes, (u8*)m_Pixels);
    }

    Image::Image(const Path& Filepath, const Format Format) : m_Format(Format)
    {
        stbi_set_flip_vertically_on_load(true);
        switch (m_Format)
        {
        case Format::R8G8B8A8_UNORM:
            m_Pixels = stbi_load(Filepath.string().c_str(), (i32*)&m_Width, (i32*)&m_Height, nullptr, 4);
            break;
        
        case Format::R16G16B16A16_USHORT:
            m_Pixels = stbi_load_16(Filepath.string().c_str(), (i32*)&m_Width, (i32*)&m_Height, nullptr, 4);
            break;
        
        case Format::R32G32B32A32_FLOAT:
            m_Pixels = stbi_loadf(Filepath.string().c_str(), (i32*)&m_Width, (i32*)&m_Height, nullptr, 4);
            break;
        default: Assert(false, "This format is not supported");
        }
    }

    Image::Image(const Buffer<u8>& Buffer, const Format Format) : m_Format(Format)
    {
        stbi_set_flip_vertically_on_load(true);
        switch (m_Format)
        {
        case Format::R8G8B8A8_UNORM:
            m_Pixels = stbi_load_from_memory(Buffer.Data(), (int)Buffer.Count(), (i32*)&m_Width, (i32*)&m_Height, nullptr, 4);
            break;
        
        case Format::R16G16B16A16_USHORT:
            m_Pixels = stbi_load_16_from_memory(Buffer.Data(), (int)Buffer.Count(), (i32*)&m_Width, (i32*)&m_Height, nullptr, 4);
            break;
        
        case Format::R32G32B32A32_FLOAT:
            m_Pixels = stbi_loadf_from_memory(Buffer.Data(), (int)Buffer.Count(), (i32*)&m_Width, (i32*)&m_Height, nullptr, 4);
            break;
        default: Assert(false, "This format is not supported");
        }
    }

    Image::Image(const BufferView<u8>& Buffer, const Format Format) : m_Format(Format)
    {
        stbi_set_flip_vertically_on_load(true);
        switch (m_Format)
        {
        case Format::R8G8B8A8_UNORM:
            m_Pixels = stbi_load_from_memory(Buffer.Data(), (int)Buffer.Count(), (i32*)&m_Width, (i32*)&m_Height, nullptr, 4);
            break;
        
        case Format::R16G16B16A16_USHORT:
            m_Pixels = stbi_load_16_from_memory(Buffer.Data(), (int)Buffer.Count(), (i32*)&m_Width, (i32*)&m_Height, nullptr, 4);
            break;
        
        case Format::R32G32B32A32_FLOAT:
            m_Pixels = stbi_loadf_from_memory(Buffer.Data(), (int)Buffer.Count(), (i32*)&m_Width, (i32*)&m_Height, nullptr, 4);
            break;
        default: Assert(false, "This format is not supported");
        }
    }
    
    Image::~Image()
    {
        stbi_image_free(m_Pixels);
    }

    u32 Image::GetWidth() const
    {
        return m_Width;
    }

    u32 Image::GetHeight() const
    {
        return m_Height;
    }

    size_t Image::GetSize() const
    {
        return m_Width * m_Height * GetBytesPerPixel(m_Format);
    }

    const void* Image::GetData() const
    {
        return m_Pixels;
    }

    void* Image::GetData()
    {
        return m_Pixels;
    }

    bool Image::Valid() const
    {
        return m_Pixels && m_Width && m_Height;
    }

    Format Image::GetFormat() const
    {
        return m_Format;
    }
}


