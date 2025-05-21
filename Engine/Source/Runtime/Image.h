#pragma once
#include "Containers/Buffer.h"
#include "Filesystem.h"
#include "Format.h"
#include <CoreExport.h>

namespace Nova {

	class CORE_API Image
	{
	public:
		Image(u32 Width, u32 Height, Format Format, const void* Pixels);
		Image(const Path& Filepath, Format Format);
		Image(const Buffer<u8>& Buffer, Format Format);
		Image(const BufferView<u8>& Buffer, Format Format);
		~Image();

		Format GetFormat() const;
		u32 GetWidth() const;
		u32 GetHeight() const;
		size_t GetSize() const;
		const void* GetData() const;
		void* GetData();
		bool Valid() const;
	
	private:
		u32 m_Width = 0, m_Height = 0;
		Format m_Format{Format::None};
		void* m_Pixels = nullptr;
	};
}
