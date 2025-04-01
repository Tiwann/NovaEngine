#pragma once
#include "Containers/StaticArray.h"
#include "Containers/Function.h"
#include "Runtime/Assertion.h"
#include "Runtime/Types.h"

namespace Nova
{
    template<typename T, size_t Size>
    class BumpAllocator
    {
    public:
        using ArrayType = StaticArray<T, Size>;
        using SizeType = typename ArrayType::SizeType;
        using ValueType = typename ArrayType::ValueType;
        using PointerType = typename ArrayType::PointerType;
        using ConstPointerType = typename ArrayType::ConstPointerType;
        using ReferenceType = typename ArrayType::ReferenceType;
        using ConstReferenceType = typename ArrayType::ConstReferenceType;
        using Predicate = typename ArrayType::Predicate;
        template<typename Out>
        using Selector = typename ArrayType::template Selector<Out>;
        
        static constexpr u32 UninitializedValue = NOVA_UNINITIALIZED;

        BumpAllocator()
        {
            m_AvailableFlags.Fill(true);
            Memory::Memset(m_Data.Data(), 0, m_Data.Count());
        }

        ~BumpAllocator()
        {
            for (const Array<PointerType> All = GetAllValid(); PointerType Element : All)
            {
                Free(Element);
            }
            m_AvailableFlags.Fill(false);
        }
        
        PointerType GetAvailableSpace()
        {
            for(SizeType i = 0; i < Size; ++i)
            {
                if(!m_AvailableFlags[i])
                    continue;
                
                m_AvailableFlags[i] = false;
                ++m_Count;
                return &m_Data[i];
            }
            return nullptr;
        }

        template<typename... Args>
        PointerType New(Args&&... Arguments)
        {
            PointerType Where = GetAvailableSpace();
            if(!Where)
            {
                Assert(false, "BumpAllocator::New No space available");
            }
            return std::construct_at(Where, std::forward<Args>(Arguments)...);
        }

        void Free(PointerType Ptr)
        {
            if(!Ptr) return;
            Assert(Ptr >= &m_Data[0] && Ptr < &m_Data[0] + Size, "Memory is not allocated from this bump allocator!");
            Ptr->~T();
            SizeType Index = Ptr - &m_Data[0];
            m_AvailableFlags[Index] = true;
            --m_Count;
            (void)Memory::Memset(Ptr, 0x00, 1);
        }

        SizeType Count() const
        {
            return m_Count;
        }

        Array<PointerType> GetAllValid()
        {
            Array<PointerType> Result;
            for(SizeType i = 0; i < Size; ++i)
            {
                if(!m_AvailableFlags[i])
                    Result.Add(&m_Data[i]);
            }
            return Result;
        }

        PointerType Single(const Predicate& Predicate)
        {
            for(SizeType i = 0; i < Size; ++i)
            {
                if(!m_AvailableFlags[i])
                {
                    if(Predicate(m_Data[i]))
                        return &m_Data[i];
                }
            }
            return nullptr;
        }

        Array<PointerType> Where(const Predicate& Predicate)
        {
            Array<PointerType> Result;
            for(SizeType i = 0; i < Size; ++i)
            {
                if(!m_AvailableFlags[i] && Predicate(m_Data[i]))
                    Result.Add(&m_Data[i]);
            }
            return Result;
        }

        template<typename Out>
        Array<Out*> Select(const Selector<Out>& Selector)
        {
            if(!Selector) return {};
            Array<Out*> Result;
            for(SizeType i = 0; i < m_Count; ++i)
            {
                if(!m_AvailableFlags[i])
                    Result.Add(Selector(m_Data[i]));
            }
            return Result;
        }
    
    private:
        StaticArray<bool, Size> m_AvailableFlags;
        ArrayType m_Data;
        SizeType m_Count = 0;
    };
}