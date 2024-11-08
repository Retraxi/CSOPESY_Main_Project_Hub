#pragma once
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>

class FlatMemoryAllocator :public IMemoryAllocator {
public:
	FlatMemoryAllocator(size_t maximumSize);
	FlatMemoryAllocator() = default;
	void* allocate(size_t size) override;
	void deallocate(void* ptr) override;
	std::string visualizeMemory() override;

private:
	size_t maximumSize;
	size_t allocatedSize;
	std::vector<char> memory; //no byte datatype
	std::unordered_map<size_t, bool> allocationMap;

	void initializeMemory();

	bool canAllocateAt(size_t index, size_t size) const;
	void allocateAt(size_t index, size_t size);
	void deallocateAt(size_t index);

};