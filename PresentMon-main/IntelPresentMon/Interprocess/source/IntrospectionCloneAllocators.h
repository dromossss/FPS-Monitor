#pragma once
#include <memory>
#include "../../CommonUtilities//Memory.h"

namespace pmon::ipc::intro
{
	template<typename T>
	class ProbeAllocator
	{
		template<typename T2>
		friend class ProbeAllocator;
	public:
		using ProbeTag = std::true_type;
		using value_type = T;
		ProbeAllocator() = default;
		ProbeAllocator(const ProbeAllocator<void>& other)
			: pTotalSize(other.pTotalSize)
		{}
		ProbeAllocator& operator=(const ProbeAllocator&) = delete;
		T* allocate(size_t count)
		{
			*pTotalSize += sizeof(T) * count + util::GetPadding<T>(*pTotalSize);
			return nullptr;
		}
		void deallocate(T*);
		size_t GetTotalSize() const
		{
			return *pTotalSize;
		}
	private:
		std::shared_ptr<size_t> pTotalSize = std::make_shared<size_t>();
	};

	template<typename T>
	class BlockAllocator
	{
		template<typename T2>
		friend class BlockAllocator;
	public:
		using value_type = T;
		BlockAllocator(size_t nBytes) : pBytes{ reinterpret_cast<char*>(malloc(nBytes)) } {}
		BlockAllocator(const BlockAllocator<void>& other)
			:
			pTotalSize(other.pTotalSize),
			pBytes{ other.pBytes }
		{}
		BlockAllocator& operator=(const BlockAllocator&) = delete;
		T* allocate(size_t count)
		{
			*pTotalSize += util::GetPadding<T>(*pTotalSize);
			const auto pStart = reinterpret_cast<T*>(pBytes + *pTotalSize);
			*pTotalSize += sizeof(T) * count;
			return pStart;
		}
	private:
		std::shared_ptr<size_t> pTotalSize = std::make_shared<size_t>();
		char* pBytes = nullptr;
	};
}