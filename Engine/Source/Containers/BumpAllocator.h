#pragma once
#include "StaticArray.h"
#include "Function.h"
#include "Runtime/Assertion.h"

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
        
        static constexpr uint32_t UninitializedValue = 0xFFFFFFFF;

        BumpAllocator()
        {
            m_AvailableFlags.Fill(true);
            Memory::Memset(m_Data.Data(), 0, m_Data.Count());
        }

        ~BumpAllocator()
        {
            for (const Array<PointerType> all = GetAllValid(); PointerType element : all)
            {
                Free(element);
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
        PointerType New(Args&&... args)
        {
            PointerType where = GetAvailableSpace();
            if(!where)
            {
                NOVA_ASSERT(false, "BumpAllocator::New No space available");
                return nullptr;
            }
            return std::construct_at(where, std::forward<Args>(args)...);
        }

        void Free(PointerType ptr)
        {
            if(!ptr) return;
            NOVA_ASSERT(ptr >= &m_Data[0] && ptr < &m_Data[0] + Size, "Memory is not allocated from this bump allocator!");
            ptr->~T();
            SizeType index = ptr - &m_Data[0];
            m_AvailableFlags[index] = true;
            --m_Count;
            (void)Memory::Memset(ptr, 0x00, 1);
        }

        SizeType Count() const
        {
            return m_Count;
        }

        Array<PointerType> GetAllValid()
        {
            Array<PointerType> result;
            for(SizeType i = 0; i < Size; ++i)
            {
                if(!m_AvailableFlags[i])
                    result.Add(&m_Data[i]);
            }
            return result;
        }

        PointerType Single(const Predicate& predicate)
        {
            for(SizeType i = 0; i < Size; ++i)
            {
                if(!m_AvailableFlags[i])
                {
                    if(predicate(m_Data[i]))
                        return &m_Data[i];
                }
            }
            return nullptr;
        }

        Array<PointerType> Where(const Predicate& predicate)
        {
            Array<PointerType> result;
            for(SizeType i = 0; i < Size; ++i)
            {
                if(!m_AvailableFlags[i] && predicate(m_Data[i]))
                    result.Add(&m_Data[i]);
            }
            return result;
        }

        template<typename Out>
        Array<Out*> Select(const Selector<Out>& selector)
        {
            if(!selector) return {};
            Array<Out*> result;
            for(SizeType i = 0; i < m_Count; ++i)
            {
                if(!m_AvailableFlags[i])
                    result.Add(selector(m_Data[i]));
            }
            return result;
        }
    
    private:
        StaticArray<bool, Size> m_AvailableFlags;
        ArrayType m_Data;
        SizeType m_Count = 0;
    };
}